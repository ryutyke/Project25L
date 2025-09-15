// Default
#include "Animation/AnimNotify/AnimNotify_EquipWeapon.h"

// Game
#include "Character/DSArmedCharacter.h"
#include "DSLogChannels.h"

void UAnimNotify_EquipWeapon::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

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
