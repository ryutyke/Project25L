// Default
#include "Animation/AnimNotify/AnimNotifyState_ComboCheck.h"

// Game
#include "Character/Characters/DSCharacter.h"
#include "GameData/Animation/DSAttackAnimMetaData.h"

void UAnimNotifyState_ComboCheck::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (nullptr == MeshComp)
	{
		return;
	}
	AActor* Owner = MeshComp->GetOwner();
	if (nullptr == Owner)
	{
		return;
	}

	ADSCharacter* Character = Cast<ADSCharacter>(Owner);
	if (false == IsValid(Character))
	{
		return;
	}

	Character->SetNextComboCommand(false);
}

void UAnimNotifyState_ComboCheck::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (nullptr == MeshComp)
	{
		return;
	}
	AActor* Owner = MeshComp->GetOwner();
	if (nullptr == Owner)
	{
		return;
	}

	ADSCharacter* Character = Cast<ADSCharacter>(Owner);

	if (false == IsValid(Character))
	{
		return;
	}

	UAnimMontage* AnimMontage = Cast<UAnimMontage>(Animation);

	if (IsValid(AnimMontage))
	{
		for (const auto MetaData : AnimMontage->GetMetaData())
		{
			UDSAttackAnimMetaData* AnimMetaData = Cast<UDSAttackAnimMetaData>(MetaData);

			if (IsValid(AnimMetaData))
			{
				Character->ProgressCombo(AnimMetaData->GetSkillType(), AnimMetaData->GetMaxComboCount());
			}
		}
	}
}
