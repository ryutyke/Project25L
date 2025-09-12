// Default
#include "Skill/Girl/DSUltimateSkill_Girl.h"

// Game
#include "Character/Characters/DSCharacter.h"

UDSUltimateSkill_Girl::UDSUltimateSkill_Girl(const FObjectInitializer& ObjectInitalize)
	: Super(ObjectInitalize)
{
	bSkillHasDuration = true;
	bRetriggerInstancedSkill = false;
	InstancingPolicy = ESkillInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = ESkillNetExecutionPolicy::LocalPredicted;
	ReplicationPolicy = ESkillReplicationPolicy::ReplicateYes;
}

void UDSUltimateSkill_Girl::ActivateSkill(const FDSSkillSpecHandle Handle, const FDSSkillActorInfo* ActorInfo)
{
	Super::ActivateSkill(Handle, ActorInfo);

	if (nullptr != ActorInfo)
	{
		ADSCharacter* Character = Cast<ADSCharacter>(ActorInfo->SkillOwner.Get());

		if (IsValid(Character))
		{
			Character->SetCanFly(true);
		}
	}
}

void UDSUltimateSkill_Girl::EndSkill(const FDSSkillSpecHandle Handle, const FDSSkillActorInfo* ActorInfo, bool bReplicateEndSkill, bool bWasCancelled)
{
	Super::EndSkill(Handle, ActorInfo, true, true);
	if (nullptr != ActorInfo)
	{
		ADSCharacter* Character = Cast<ADSCharacter>(ActorInfo->SkillOwner.Get());

		if (IsValid(Character))
		{
			Character->SetCanFly(false);
		}
	}
}
