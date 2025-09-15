// Default
#include "Skill/Boy/DSPrimarySkill_Boy.h"

// Game
#include "Character/Characters/DSCharacter_Boy.h"
#include "Components/Skill/DSSkillControlComponent.h"
#include "DSLogChannels.h"

UDSPrimarySkill_Boy::UDSPrimarySkill_Boy(const FObjectInitializer& ObjectInitalize)
	: Super(ObjectInitalize)
{
	CooldownPolicy = ESkillCooldownPolicy::None;
	InstancingPolicy = ESkillInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = ESkillNetExecutionPolicy::LocalOnly;
	//ReplicationPolicy = ESkillReplicationPolicy::ReplicateYes;
	ReplicationPolicy = ESkillReplicationPolicy::ReplicateNo;
	NetSecurityPolicy = ESkillNetSecurityPolicy::ServerOnlyTermination;

	bRetriggerInstancedSkill = true;
}

void UDSPrimarySkill_Boy::ActivateSkill(const FDSSkillSpecHandle Handle, const FDSSkillActorInfo* ActorInfo)
{
	Super::ActivateSkill(Handle, ActorInfo);

	DS_LOG(DSSkillLog, Log, TEXT("%s"), *UEnum::GetValueAsString(CurrentSkillType));
	if (nullptr != ActorInfo)
	{
		ADSCharacter_Boy* BoyCharacter = Cast<ADSCharacter_Boy>(ActorInfo->SkillOwner.Get());

		if (IsValid(BoyCharacter))
		{
			BoyCharacter->BeginPrimarySkill();
		}
	}
}
