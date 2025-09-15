// Default
#include "Skill/Girl/DSSkill1_Girl.h"

// UE
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"

// Game
#include "Gimmick/DSGridPlane.h"
#include "GameData/Physics/DSCollision.h"
#include "Character/DSArmedCharacter.h"
#include "Weapon/DSGun.h"
#include "Components/Skill/DSSkillControlComponent.h"
#include "GameData/Skill/DSSkillAttribute.h"
#include "DSLogChannels.h"

UDSSkill1_Girl::UDSSkill1_Girl(const FObjectInitializer& ObjectInitalize)
	: Super(ObjectInitalize)
	, PreGrid()
	, PreTargetLocation()
	, Weapon(nullptr)
	, GridPlane(nullptr)
	, NiagaraEffect(nullptr)
	, bIsInstalled(false)
	, GravityStatus(EDSGravityStatus::None)
	, CancelSkillTimer()
	, GravityImpulse(150.f)
{
	bRetriggerInstancedSkill = true;
	InstancingPolicy = ESkillInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = ESkillNetExecutionPolicy::ServerInitiated;
	ReplicationPolicy = ESkillReplicationPolicy::ReplicateYes;
}

bool UDSSkill1_Girl::CanActivateSkill(const FDSSkillSpecHandle Handle, const FDSSkillActorInfo* ActorInfo)
{
	bool bResult = Super::CanActivateSkill(Handle, ActorInfo);

	return bResult;
}

void UDSSkill1_Girl::ActivateSkill(const FDSSkillSpecHandle Handle, const FDSSkillActorInfo* ActorInfo)
{
	Super::ActivateSkill(Handle, ActorInfo);

	switch (GravityStatus)
	{
	case EDSGravityStatus::None:
		GravityStatus = EDSGravityStatus::Installed;
		SpawnGravity();
		break;
	case EDSGravityStatus::Confirm:
		//확정한다.
		GravityStatus = EDSGravityStatus::Activate; //발동시킨다.
		EndSkill(Handle, ActorInfo, true, true); //발동했음을 알리고 종료한다.
		break;
	
	case EDSGravityStatus::Installed:
		//확정으로 상태를 변화시키고, return false 처리를 통해서 EndSkill을 호출하지 못하도록 한다.
		APlayerController* PlayerController = ActorInfo->PlayerController.Get();

		if (IsValid(PlayerController))
		{
			//Q 스킬의 입력이 더이상 안들어온다면 취소하기 위해서 N초간 타이머를 설정한다.
			UWorld* World = PlayerController->GetWorld();
			check(World);

			World->GetTimerManager().SetTimer(CancelSkillTimer, this, &UDSSkill1_Girl::CancelGravity, SkillWaitTime, false);
		}
		GravityStatus = EDSGravityStatus::Confirm;
		return;
	}
}

void UDSSkill1_Girl::InternalTick(float DeltaTime)
{

	if (GravityStatus == EDSGravityStatus::Installed)
	{
		if (Weapon.IsValid())
		{
			FHitResult HitResult;

			bool bIsSuccess = Weapon->TryInstallInRange(HitResult, CCHANNEL_DSINSTALLGRAVITY, AutoAimAngle, SkillDistance);

			if (false == bIsSuccess)
			{
				//실패
				//나이아가라를 안보이게 한다.
				if (IsValid(NiagaraEffect))
				{
					NiagaraEffect->SetVisibility(false);
				}
				return;
			}

			if (false == GridPlane.IsValid())
			{
				return;
			}

			const FIntPoint& GridPoint = GridPlane->GetGridCell(HitResult.Location);
			FVector TargetLocation;
			if (IsValid(NiagaraEffect))
			{
				NiagaraEffect->SetVisibility(true);

				//보간이 필요함. 그렇지 않다면 두두두 흔들리는 현상이 존재함.
				//실제 TargetLocation의 변화가 거의 없다면, 무시한다.
				TargetLocation = GridPlane->GetGridCellCenter(GridPoint);
				TargetLocation.Z = HitResult.Location.Z;
				if (GridPoint != PreGrid || FVector::Dist(PreTargetLocation, TargetLocation) > 50.f)
				{
					FVector Location = FMath::VInterpTo(PreTargetLocation, TargetLocation, DeltaTime, 10.f);
					NiagaraEffect->SetWorldLocation(Location);

					PreTargetLocation = Location;
				}
			}
			PreGrid = GridPoint;
		}
	}

}

void UDSSkill1_Girl::EndSkill(const FDSSkillSpecHandle Handle, const FDSSkillActorInfo* ActorInfo, bool bReplicateEndSkill, bool bWasCancelled)
{
	Super::EndSkill(Handle, ActorInfo, bReplicateEndSkill, bWasCancelled);

	if (EDSGravityStatus::Activate == GravityStatus)
	{
		/*확정되어서 중력건이 실행되는 상태*/
		ConfirmGravity();
	}
	
	if (IsValid(NiagaraEffect))
	{
		//확정이든 취소든 나이아가라를 숨긴다.
		NiagaraEffect->DestroyComponent();
	}

	CancelSkillTimer.Invalidate();
	bIsInstalled = false;
	ElapsedTime = 0.f;
	GravityStatus = EDSGravityStatus::None;
}

void UDSSkill1_Girl::SpawnGravity()
{
	ADSArmedCharacter* Character = Cast<ADSArmedCharacter>(CurrentActorInfo->SkillOwner.Get());
	if (false == IsValid(Character))
	{
		return;
	}

	Weapon = Cast<ADSGun>(Character->GetWeapon());

	if (false == Weapon.IsValid())
	{
		return;
	}

	FHitResult HitResult;

	bool bIsSuccess = Weapon->TryInstallInRange(HitResult, CCHANNEL_DSINSTALLGRAVITY, AutoAimAngle, SkillDistance);

	if (false == bIsSuccess)
	{
		return;
	}

	GridPlane = Cast<ADSGridPlane>(HitResult.GetActor());

	if (false == GridPlane.IsValid())
	{
		return;
	}

	const FIntPoint& GridPoint = GridPlane->GetGridCell(HitResult.Location);

	if (Effects.Contains(ESkillActivationStatus::Before))
	{
		//스킬 누르는 순간, VFX를 스폰한다.
		UWorld* World = Character->GetWorld();
		check(World);
		FVector TargetLocation = GridPlane->GetGridCellCenter(GridPoint);
		TargetLocation.Z = HitResult.Location.Z; //바닥 Z값으로 초기화

		NiagaraEffect = UNiagaraFunctionLibrary::SpawnSystemAtLocation(World, Effects[ESkillActivationStatus::Before].LoadSynchronous(), TargetLocation, FRotator::ZeroRotator, FVector(2.0f, 2.0f, 2.f), false);

		PreTargetLocation = TargetLocation;
	}
}

void UDSSkill1_Girl::ConfirmGravity()
{
	if (false == Weapon.IsValid())
	{
		return;
	}

	if (Effects.Contains(ESkillActivationStatus::InProgress))
	{
		Weapon->AttackSkill1(Effects[ESkillActivationStatus::InProgress], PreTargetLocation, SkillRadius, GravityImpulse, SkillDamage, SkillDuration);
	}
}

void UDSSkill1_Girl::CancelGravity()
{
	EndSkill(CurrentSpecHandle, CurrentActorInfo, true, true);
}