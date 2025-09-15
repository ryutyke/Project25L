// Default
#include "Character/DSArmedCharacter.h"

// UE
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"

// Game
#include "System/DSGameDataSubsystem.h"
#include "GameData/DSWeaponData.h"
#include "Weapon/DSWeapon.h"
#include "GameData/Skill/DSSkillAttribute.h"
#include "Components/Skill/DSSkillControlComponent.h"
#include "Components/DSStatComponent.h"
#include "Game/DSGameState.h"


ADSArmedCharacter::ADSArmedCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, bIsEquipped(false)
	, Weapon(nullptr)
{
}

void ADSArmedCharacter::BeginPlay()
{
	Super::BeginPlay();

	LoadWeapon();
}

void ADSArmedCharacter::LoadWeapon()
{
	UDSGameDataSubsystem* DataManager = UDSGameDataSubsystem::Get(this);

	check(DataManager);

	FDSWeaponData* WeaponData = DataManager->GetDataRowByEnum<FDSWeaponData, EWeaponType>(EDataTableType::WeaponData, WeaponType);

	if (nullptr != WeaponData)
	{
		if (HasAuthority())
		{
			TSoftClassPtr<ADSWeapon> WeaponMesh = WeaponData->Weapon;

			UDSGameDataSubsystem::StreamableManager.RequestAsyncLoad(WeaponMesh.ToSoftObjectPath(), FStreamableDelegate::CreateLambda([WeakPtr = TWeakObjectPtr<ADSArmedCharacter>(this), WeaponMesh]()
				{
					if (WeakPtr.IsValid())
					{
						UWorld* World = WeakPtr->GetWorld();

						check(World);

						ADSArmedCharacter* Character = WeakPtr.Get();
						TSubclassOf<ADSWeapon> WeaponClass = WeaponMesh.Get();

						if (IsValid(Character))
						{
							FActorSpawnParameters Params;
							Params.Owner = Character;

							Character->Weapon = World->SpawnActor<ADSWeapon>(WeaponClass, Character->WeaponRelativeTransform.GetLocation(), Character->WeaponRelativeTransform.GetRotation().Rotator(), Params);
							Character->Weapon->AttachToComponent(Character->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, WeakPtr->SocketName[EWeaponSocketType::Stow]);
						}
					}
				}));
		}

		TArray<FSoftObjectPath> MontageToStream;
		
		for(const auto Montage : WeaponData->Montages)
		{
			MontageToStream.AddUnique(Montage.Value.ToSoftObjectPath());
		}

		UDSGameDataSubsystem::StreamableManager.RequestAsyncLoad(MontageToStream, FStreamableDelegate::CreateLambda(([WeakPtr = TWeakObjectPtr<ADSArmedCharacter>(this), WeaponData]()
			{
				if (WeakPtr.IsValid())
				{
					if (nullptr != WeaponData)
					{
						//애니메이션은 둘 다 가지고 온다.
						for (const auto Montage : WeaponData->Montages)
						{
							WeakPtr->WeaponMontages.Add(Montage.Key, Montage.Value.Get());
						}
					}
				}
			})));
	}
}

void ADSArmedCharacter::PreAttackHitCheck(UAnimMontage* Anim)
{
	Super::PreAttackHitCheck(Anim);

	if (IsValid(Weapon))
	{
		Weapon->PreAttackHitCheck(Anim);
	}
}

void ADSArmedCharacter::AttackHitCheck(const ESkillType& SkillTag)
{
	// Super 의도적으로 호출하지 않음.

	UDSGameDataSubsystem* DataSubsystem = UDSGameDataSubsystem::Get(this);
	check(DataSubsystem);

	EDataTableType DataTableType = UDSSkillControlComponent::GetSkillDataTableTypeByCharacter(CharacterType, INDEX_NONE);

	FDSSkillAttribute* SkillData;
	SkillData = DataSubsystem->GetDataRowByEnum<FDSSkillAttribute, ESkillType>(DataTableType, SkillTag);

	if (nullptr == SkillData)
	{
		DS_LOG(DSStatLog, Warning, TEXT("Skill Attribute Data not found!"));
		return;
	}
	
	float SkillRange = SkillData->SkillRadius;
	float SkillDamage = GetStatComponent()->GetFinalSkillDamage(SkillTag);

	if (IsValid(Weapon))
	{
		Weapon->AttackHitCheck(SkillTag, SkillRange, SkillDamage);
	}
}

void ADSArmedCharacter::PostAttackHitCheck()
{
	// Super 의도적으로 호출하지 않음.

	if (IsValid(Weapon))
	{
		Weapon->PostAttackHitCheck();
	}
}

void ADSArmedCharacter::PlayWeaponActionMontage(EWeaponState WeaponState)
{
	if (false == HasAuthority())
	{
		PlayAnimation(WeaponState);
	}

	//애니메이션을 실행한다.
	ServerRPC_PlayWeaponAction(WeaponState);
}

void ADSArmedCharacter::MoveEquip()
{
	EWeaponSocketType SocketType = EWeaponSocketType::Stow;

	if (bIsEquipped)
	{
		SocketType = EWeaponSocketType::Stow;
	}
	else
	{
		SocketType = EWeaponSocketType::Equipped;
	}

	if (IsValid(Weapon))
	{
		if (SocketName.Contains(SocketType))
		{
			Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, SocketName[SocketType]);
		}
	}
	
	bIsEquipped = !bIsEquipped;
}


void ADSArmedCharacter::PlayAnimation(EWeaponState WeaponState)
{
	if (false == WeaponMontages.Contains(WeaponState))
	{
		return;
	}

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (IsValid(AnimInstance))
	{
		if (AnimInstance->IsAnyMontagePlaying())
		{
			//현재 애니메이션 몽타주가 실행중임으로 리턴한다.
			return;
		}
	}

	if (IsValid(WeaponMontages[WeaponState]))
	{
		PlayAnimMontage(WeaponMontages[WeaponState]);
	}
}

void ADSArmedCharacter::MulticastRPC_PlayAnimation_Implementation(EWeaponState WeaponState)
{
	if (false == HasAuthority() && IsLocallyControlled())
	{
		return;
	}

	if (false == WeaponMontages.Contains(WeaponState))
	{
		return;
	}

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (IsValid(AnimInstance))
	{
		if (AnimInstance->IsAnyMontagePlaying())
		{
			//현재 애니메이션 몽타주가 실행중임으로 리턴한다.
			return;
		}
	}

	if (IsValid(WeaponMontages[WeaponState]))
	{
		PlayAnimMontage(WeaponMontages[WeaponState]);
	}
}

void ADSArmedCharacter::ServerRPC_PlayWeaponAction_Implementation(EWeaponState EquipState)
{
	ReceivedComboCommandCnt = 0;

	CurrentComboIndex = 0;
	bIsProgressCombo = true;

	// 재생 못 하는 상태면 ClientRPC_CancelAnimation()

	MulticastRPC_PlayAnimation(EquipState);	
}

void ADSArmedCharacter::ClientRPC_StopAnimation_Implementation(EWeaponState WeaponState)
{
	if (false == WeaponMontages.Contains(WeaponState))
	{
		return;
	}

	UAnimMontage* WeaponMontage = WeaponMontages[WeaponState];

	if (false == IsValid(WeaponMontage))
	{
		return;
	}

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (false == IsValid(AnimInstance))
	{
		return;
	}

	if (AnimInstance->Montage_IsPlaying(WeaponMontage))
	{
		AnimInstance->Montage_Stop(0.f, WeaponMontage);
	}
}

void ADSArmedCharacter::ClientRPC_SyncComboEndMontage_Implementation(EWeaponState WeaponState, float AnimPos, double InServerWorldTime)
{
	// 알아서 종료되게
	if (false == bIsProgressCombo)
	{
		return;
	}
	bIsProgressCombo = false;

	if (false == WeaponMontages.Contains(WeaponState))
	{
		return;
	}

	UAnimMontage* WeaponMontage = WeaponMontages[WeaponState];

	if (false == IsValid(WeaponMontage))
	{
		return;
	}

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (false == IsValid(AnimInstance))
	{
		return;
	}

	UWorld* World = GetWorld();

	if (nullptr == World)
	{
		return;
	}

	AGameStateBase* GameState = World->GetGameState();

	if (nullptr == GameState)
	{
		return;
	}

	double ServerWorldTime = GameState->GetServerWorldTimeSeconds();
	const double NetworkDelay = FMath::Max(0.0, ServerWorldTime - InServerWorldTime);
	float TargetPos = AnimPos + static_cast<float>(NetworkDelay);

	const float MontageLength = WeaponMontage->GetPlayLength();

	TargetPos = FMath::Clamp(TargetPos, 0.f, MontageLength - KINDA_SMALL_NUMBER);

	if (false == AnimInstance->Montage_IsPlaying(WeaponMontage))
	{
		AnimInstance->Montage_Play(WeaponMontage);
	}

	AnimInstance->Montage_SetPosition(WeaponMontage, TargetPos);
}

void ADSArmedCharacter::ClientRPC_PlayWeaponAction_Implementation(ADSArmedCharacter* Character, EWeaponState EquipState)
{
	if (IsValid(Character))
	{
		Character->PlayAnimation(EquipState);
	}
}

void ADSArmedCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADSArmedCharacter, Weapon);
}
