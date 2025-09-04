// Default
#include "Skill/Mister/DSPrimarySkill_Mister.h"

// Game
#include "Components/Skill/DSSkillControlComponent_Mister.h"
#include "Character/Characters/DSCharacter.h"
#include "DSLogChannels.h"

UDSPrimarySkill_Mister::UDSPrimarySkill_Mister(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, AnimMontage()
{
	CooldownPolicy = ESkillCooldownPolicy::None;
	bSkillHasDuration = false;
	InstancingPolicy = ESkillInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = ESkillNetExecutionPolicy::LocalPredicted;
	ReplicationPolicy = ESkillReplicationPolicy::ReplicateYes;
}

bool UDSPrimarySkill_Mister::CanActivateSkill(const FDSSkillSpecHandle Handle, const FDSSkillActorInfo* ActorInfo)
{
	bool bResult = Super::CanActivateSkill(Handle, ActorInfo);

	if (nullptr == ActorInfo)
	{
		return false;
	}

	ADSCharacter* Character = Cast<ADSCharacter>(ActorInfo->SkillOwner.Get());
	if (false == IsValid(Character))
	{
		return false;
	}

	//실제 로드해야할 Punch 몽타주
	if (false == IsValid(AnimMontage))
	{
		AnimMontage = AnimMontageClass.LoadSynchronous();
	}

	return bResult;
}


void UDSPrimarySkill_Mister::ActivateSkill(const FDSSkillSpecHandle Handle, const FDSSkillActorInfo* ActorInfo)
{
	Super::ActivateSkill(Handle, ActorInfo);

	if (nullptr == ActorInfo)
	{
		return;
	}
	UDSSkillControlComponent_Mister* SkillControlComponent = Cast<UDSSkillControlComponent_Mister>(ActorInfo->SkillControlComponent);
	if (false == IsValid(SkillControlComponent))
	{
		return;
	}
	ADSCharacter* Character = Cast<ADSCharacter>(ActorInfo->SkillOwner.Get());
	if (false == IsValid(Character))
	{
		return;
	}
	UAnimInstance* AnimInstance = ActorInfo->GetAnimInstance();

	if (false == IsValid(AnimInstance))
	{
		return;
	}

	if (AnimInstance->Montage_IsPlaying(AnimMontage))
	{
		return;
	}

	int32 SectionIndexToPlay = Character->GetNextComboIndex();

	if (SectionIndexToPlay <= 0)
	{
		//시작
		SkillControlComponent->AttackPrimarySkill(AnimMontage, FName());
	}
	else
	{
		bool bComboInputReceived = Character->GetNextComboCommand();
		if (bComboInputReceived)
		{
			//콤보 입력이 들어왔음.
			FName NextSection = *FString::Printf(TEXT("%s%d"), *MontageSectionNamePrefix, SectionIndexToPlay);
			SkillControlComponent->AttackPrimarySkill(AnimMontage, NextSection);
		}
	}
}
