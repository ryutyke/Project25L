// Default
#include "Animation/AnimNotify/ANS_SetComboTiming.h"

// Game
#include "Character/Characters/DSCharacter.h"

void UANS_SetComboTiming::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
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

	Character->SetComboInputTiming(true);
}

void UANS_SetComboTiming::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
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

	Character->SetComboInputTiming(false);
}
