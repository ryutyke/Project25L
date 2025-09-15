// Default
#include "DSSkill2_Girl.h"

// UE
#include "Kismet/KismetSystemLibrary.h"

// Game
#include "Character/DSArmedCharacter.h"
#include "Components/Skill/DSSkillControlComponent.h"
#include "GameData/Physics/DSCollision.h"
#include "GameData/Skill/DSSkillAttribute.h"
#include "Player/DSPlayerController.h"
#include "System/DSGameDataSubSystem.h"
#include "Weapon/DSGun.h"
#include "Gimmick/Skill/DSPortalActor.h"
#include "DSLogChannels.h"

UDSSkill2_Girl::UDSSkill2_Girl(const FObjectInitializer& ObjectInitalize)
:Super(ObjectInitalize)
{
	bSkillHasDuration = false;
	bCanTick = false;

	InstancingPolicy = ESkillInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = ESkillNetExecutionPolicy::LocalPredicted;
	ReplicationPolicy = ESkillReplicationPolicy::ReplicateYes;
	
	CurrentPortalState = EDSPortalState::None;

	bRetriggerInstancedSkill = true;
}

void UDSSkill2_Girl::ActivateSkill(const FDSSkillSpecHandle Handle, const FDSSkillActorInfo* ActorInfo)
{
	Super::ActivateSkill(Handle, ActorInfo);
	ADSArmedCharacter* Character = Cast<ADSArmedCharacter>(ActorInfo->SkillOwner.Get());

	if (false == IsValid(Character))
	{
		return;
	}
	// 소녀 캐릭터의 무기 총을 초기화한다.
	Gun = Cast<ADSGun>(Character->GetWeapon());

	if (false == IsValid(Gun))
	{
		return;
	}
	bRetriggerInstancedSkill = true;

	ADSPortalActor* Blackhole = nullptr;
	ADSPortalActor* Whitehole = nullptr;
	UWorld* World = CurrentActorInfo->SkillOwner->GetWorld();
	FTimerDelegate TimerDel;
	TimerDel.BindUObject(this, &UDSSkill2_Girl::ActivatePortal);
	
	switch (CurrentPortalState)
	{
	case EDSPortalState::None:
		Blackhole = Gun->TryPlacePortalEffect(PortalInstallationParams[EDSPortalType::Entrance], AutoAimAngle, SkillDistance, ExitLocation);
		if (IsValid(Blackhole))
		{
			Portals.Add(EDSPortalType::Entrance, Blackhole);
			CurrentPortalState = EDSPortalState::EntrancePlaced;
		}
		else
		{
			EndSkill(Handle, ActorInfo, true, true);
		}
		break;

	case EDSPortalState::EntrancePlaced:
	{
		Whitehole = Gun->TryPlacePortalEffect(PortalInstallationParams[EDSPortalType::Exit], AutoAimAngle, SkillDistance, ExitLocation);
		if (IsValid(Whitehole))
		{
			Portals.Add(EDSPortalType::Exit, Whitehole);
			CurrentPortalState = EDSPortalState::ExitPlaced;
			ActivateSkill(Handle, ActorInfo);
		}
		else
		{
			EndSkill(Handle, ActorInfo, true, true);
		}
		break;
	}
	case EDSPortalState::ExitPlaced:
		check(World);
		
		World->GetTimerManager().SetTimer(
			PortalActivateTimerHandle,
			TimerDel,
			2.0f, 
			false);

		break;
	default:
		break;
	}
}

void UDSSkill2_Girl::ActivatePortal()
{
	if(false == Portals.Contains(EDSPortalType::Entrance)  || false == Portals.Contains(EDSPortalType::Exit))
	{
		return;
	}

	if(false == IsValid(Portals[EDSPortalType::Entrance]) || false == IsValid(Portals[EDSPortalType::Exit]))
	{
		return;
	}

	Portals[EDSPortalType::Entrance]->SetExitLocation(ExitLocation);
	Portals[EDSPortalType::Entrance]->OnActivatePortal();
	Portals[EDSPortalType::Exit]->OnActivatePortal();
}

void UDSSkill2_Girl::EndSkill(const FDSSkillSpecHandle Handle, const FDSSkillActorInfo* ActorInfo, bool bReplicateEndSkill, bool bWasCancelled)
{
	if(Portals.Contains(EDSPortalType::Entrance))
	{
		if(IsValid(Portals[EDSPortalType::Entrance]))
		{
			Portals[EDSPortalType::Entrance]->OnResetPortal();
		}
	}

	if(Portals.Contains(EDSPortalType::Exit))
	{
		if(IsValid(Portals[EDSPortalType::Exit]))
		{
			Portals[EDSPortalType::Exit]->OnResetPortal();
		}
	}
	bRetriggerInstancedSkill = false;

	CurrentPortalState = EDSPortalState::None;
	
	Super::EndSkill(Handle, ActorInfo, bReplicateEndSkill, bWasCancelled);
}

void UDSSkill2_Girl::InternalTick(float DeltaTime)
{
	if (CurrentPortalState == EDSPortalState::None) 
	{
		return;
	}

	PortalElapsedTime += DeltaTime;

	if (PortalElapsedTime/TickInterval >= SkillWaitTime) 
	{
		PortalElapsedTime = 0.f;
		EndSkill(CurrentSpecHandle, CurrentActorInfo, true, true);
	}
}

void UDSSkill2_Girl::BeginPlay()
{
	Super::BeginPlay();

	CurrentPortalState = EDSPortalState::None;
}

