// Default
#include "Skill/Common/DSSkillDodge.h"

// Game
#include "Character/Characters/DSCharacter.h"
#include "System/DSGameUtils.h"

#include "DSLogChannels.h"

UDSSkillDodge::UDSSkillDodge(const FObjectInitializer& ObjectInitalize)
	: Super(ObjectInitalize)
{
	CooldownPolicy = ESkillCooldownPolicy::CooldownAfterActive;
	InstancingPolicy = ESkillInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = ESkillNetExecutionPolicy::ServerInitiated;
	ReplicationPolicy = ESkillReplicationPolicy::ReplicateYes;
	NetSecurityPolicy = ESkillNetSecurityPolicy::ClientOrServer;
}

void UDSSkillDodge::ActivateSkill(const FDSSkillSpecHandle Handle, const FDSSkillActorInfo* ActorInfo)
{
	Super::ActivateSkill(Handle, ActorInfo);

	if (nullptr == ActorInfo)
	{
		return;
	}

	ADSCharacter* Character = Cast<ADSCharacter>(ActorInfo->SkillOwner);

	if (IsValid(Character))
	{
		Character->SetIsDodge(650.f, 1000.f);
	}
}

void UDSSkillDodge::EndSkill(const FDSSkillSpecHandle Handle, const FDSSkillActorInfo* ActorInfo, bool bReplicateEndSkill, bool bWasCancelled)
{
	Super::EndSkill(Handle, ActorInfo, bReplicateEndSkill, bWasCancelled);

}
