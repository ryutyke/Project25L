// Default
#include "Animation/AnimNotify/AnimNotifyState_EquipWeapon.h"

// Game
#include "Character/DSArmedCharacter.h"
#include "DSLogChannels.h"

void UAnimNotifyState_EquipWeapon::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (false == IsValid(MeshComp))
	{
		return;
	}

	ADSArmedCharacter* Character = Cast<ADSArmedCharacter>(MeshComp->GetOwner());

	if (IsValid(Character))
	{
		Character->MoveEquip();
	}
}
