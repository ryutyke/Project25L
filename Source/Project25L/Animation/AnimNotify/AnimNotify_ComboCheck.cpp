// Default
#include "Animation/AnimNotify/AnimNotify_ComboCheck.h"

// Game
#include "AnimNotify_ComboCheck.h"
#include "Character/Characters/DSCharacter.h"
#include "GameData/Animation/DSAttackAnimMetaData.h"

UAnimNotify_ComboCheck::UAnimNotify_ComboCheck()
	: Super()
{
}

void UAnimNotify_ComboCheck::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	
	if (false == IsValid(MeshComp))
	{
		return;
	}
	AActor* Owner = MeshComp->GetOwner();
	ensure(IsValid(Owner));

	if (Owner->HasAuthority())
	{
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
}