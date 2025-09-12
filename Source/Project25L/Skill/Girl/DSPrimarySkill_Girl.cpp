// Default
#include "Skill/Girl/DSPrimarySkill_Girl.h"

// UH
#include "GameFramework/HUD.h"

// Game
#include "Character/DSArmedCharacter.h"
#include "Components/Skill/DSSkillControlComponent.h"
#include "Weapon/DSGun.h"

#include "DSLogChannels.h"
UDSPrimarySkill_Girl::UDSPrimarySkill_Girl(const FObjectInitializer& ObjectInitalize)
	: Super(ObjectInitalize)
{
	CooldownPolicy = ESkillCooldownPolicy::None;
	InstancingPolicy = ESkillInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = ESkillNetExecutionPolicy::ServerInitiated;
	ReplicationPolicy = ESkillReplicationPolicy::ReplicateYes;
	NetSecurityPolicy =  ESkillNetSecurityPolicy::ClientOrServer;
}

void UDSPrimarySkill_Girl::CallActivateSkill(const FDSSkillSpecHandle Handle, const FDSSkillActorInfo* ActorInfo)
{
	Super::CallActivateSkill(Handle, ActorInfo);
	
}

void UDSPrimarySkill_Girl::ActivateSkill(const FDSSkillSpecHandle Handle, const FDSSkillActorInfo* ActorInfo)
{
	Super::ActivateSkill(Handle, ActorInfo);

	DS_LOG(DSSkillLog, Log, TEXT("%s"), *UEnum::GetValueAsString(CurrentSkillType));
	if (nullptr != ActorInfo)
	{
		ADSArmedCharacter* Character = Cast<ADSArmedCharacter>(ActorInfo->SkillOwner.Get());

		if (IsValid(Character))
		{
			const ADSWeapon* Weapon = Cast<ADSWeapon>(Character->GetWeapon());
			
			if (false == IsValid(Weapon))
			{
				return;
			}

			Weapon->AttackPrimarySkill(AutoAimAngle, SkillDistance, SkillDamage);
		}
	}
}

void UDSPrimarySkill_Girl::EndSkill(const FDSSkillSpecHandle Handle, const FDSSkillActorInfo* ActorInfo, bool bReplicateEndSkill, bool bWasCancelled)
{
	Super::EndSkill(Handle, ActorInfo, bReplicateEndSkill, bWasCancelled);
	
}
