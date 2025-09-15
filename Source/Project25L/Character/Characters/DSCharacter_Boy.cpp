// Default
#include "Character/Characters/DSCharacter_Boy.h"

// Game
#include "Components/Skill/DSSkillControlComponent_Boy.h"
#include "Components/Input/DSPlayerInputComponent_Boy.h"

#include "DSLogChannels.h"

ADSCharacter_Boy::ADSCharacter_Boy(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SkillControlComponent = CreateDefaultSubobject<UDSSkillControlComponent_Boy>(TEXT("SkillControlComponent"));
	DSPlayerInputComponent = CreateDefaultSubobject<UDSPlayerInputComponent_Boy>(TEXT("DSPlayerInputComponent"));

}

void ADSCharacter_Boy::BeginPrimarySkill()
{
	if (false == bIsEquipped)
	{
		return;
	}
	
	UAnimInstance* AnimInstance = GetAnimInstance();
	if (IsValid(AnimInstance))
	{
		if (AnimInstance->IsAnyMontagePlaying())
		{
			return;
		}
	}

	// Init Combo attack
	InitComboAttack();
	
	PlayWeaponActionMontage(EWeaponState::PrimaryAttack);
}

void ADSCharacter_Boy::JumpToNextPrimaryAttackCombo(int32 MaxSequenceCount)
{
	if (false == WeaponMontages.Contains(EWeaponState::PrimaryAttack))
	{
		return;
	}

	UAnimMontage* ComboMontage = WeaponMontages[EWeaponState::PrimaryAttack];

	UAnimInstance* AnimInstance = GetAnimInstance();

	if (false == IsValid(AnimInstance))
	{
		return;
	}

	if (false == AnimInstance->Montage_IsPlaying(ComboMontage))
	{
		return;
	}

	SetNextComboEnable(MaxSequenceCount);
	
	FName NextSectionName = *FString::Printf(TEXT("Combo_%d"), GetNextComboIndex());
	const int32 NextSectionIndex = ComboMontage->GetSectionIndex(NextSectionName);

	AnimInstance->Montage_JumpToSection(NextSectionName, ComboMontage);
	
	if (HasAuthority())
	{
		Multicast_JumpToNextPrimaryAttackCombo(NextSectionIndex);
	}
}

void ADSCharacter_Boy::Multicast_JumpToNextPrimaryAttackCombo_Implementation(int32 NextSectionIndex)
{
	if (HasAuthority())
	{
		return;
	}

	if (false == WeaponMontages.Contains(EWeaponState::PrimaryAttack))
	{
		return;
	}

	UAnimMontage* ComboMontage = WeaponMontages[EWeaponState::PrimaryAttack];

	UAnimInstance* AnimInstance = GetAnimInstance();

	if (false == IsValid(AnimInstance))
	{
		return;
	}

	if (false == AnimInstance->Montage_IsPlaying(ComboMontage))
	{
		AnimInstance->Montage_Play(ComboMontage);
	}
	else
	{
		if ((false == HasAuthority()) && (true == IsLocallyControlled()))
		{
			return;
		}
	}

	const FName NextSectionName = ComboMontage->GetSectionName(NextSectionIndex);
	AnimInstance->Montage_JumpToSection(NextSectionName, ComboMontage);
}

void ADSCharacter_Boy::ProgressCombo(const ESkillType& SkillTag, int32 MaxSequenceCount)
{
	Super::ProgressCombo(SkillTag, MaxSequenceCount);

	if (false == bIsProgressCombo)
	{
		return;
	}

	// SentCnt is for server.
	if (GetReceivedComboCommandCnt() < 1 && false == GetNextComboCommand())
	{
		EndCombo(SkillTag);

		return;
	}

	if (true == HasAuthority() && false == IsLocallyControlled())
	{
		DecreaseReceivedComboCommandCnt();
		SetNextComboCommand(true);
	}

	if (SkillTag == ESkillType::PrimarySkill)
	{
		JumpToNextPrimaryAttackCombo(MaxSequenceCount);
	}

	SetNextComboCommand(false);
}

void ADSCharacter_Boy::InitComboAttack()
{
	CurrentComboIndex = 0;
	bIsProgressCombo = true;
}

void ADSCharacter_Boy::ServerRPC_InitComboAttack_Implementation()
{
	CurrentComboIndex = 0;
	bIsProgressCombo = true;
}

void ADSCharacter_Boy::EndCombo(const ESkillType& SkillTag)
{
	bIsProgressCombo = false;

	if (HasAuthority())
	{
		if (SkillTag == ESkillType::PrimarySkill)
		{
			UAnimMontage* ComboMontage = WeaponMontages[EWeaponState::PrimaryAttack];

			UAnimInstance* AnimInstance = GetAnimInstance();
			if (nullptr == AnimInstance)
			{
				return;
			}

			float AnimPos = AnimInstance->Montage_GetPosition(ComboMontage);
			
			UWorld* World = GetWorld();
			if (nullptr == World)
			{
				return;
			}

			double ServerTime = World->GetTimeSeconds();

			ClientRPC_SyncComboEndMontage(EWeaponState::PrimaryAttack, AnimPos, ServerTime);
		}
	}
}
