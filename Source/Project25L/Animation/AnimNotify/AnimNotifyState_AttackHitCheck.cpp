// Default
#include "Animation/AnimNotify/AnimNotifyState_AttackHitCheck.h"

// Game
#include "Character/Characters/DSCharacter.h"
#include "GameData/Animation/DSAttackAnimMetaData.h"
#include "GameData/DSEnums.h"
#include "DSLogChannels.h"
#include "GameData/Animation/BoneToComponentAnimMetaData.h"

UAnimNotifyState_AttackHitCheck::UAnimNotifyState_AttackHitCheck()
	: Super()
	, bHitCheckOnTick(false)
	, MaxSequenceCount(0)
{
}

void UAnimNotifyState_AttackHitCheck::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

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
					MaxSequenceCount = AnimMetaData->GetMaxComboCount();
					bHitCheckOnTick = AnimMetaData->GetCheckOnTick();
					SkillType = AnimMetaData->GetSkillType();

					Character->PreAttackHitCheck(AnimMontage);

					if (false == bHitCheckOnTick)
					{
						Character->AttackHitCheck(HitSocket, SkillType);
					}
				}
			}
		}
	}
}

void UAnimNotifyState_AttackHitCheck::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	if (false == IsValid(MeshComp))
	{
		return;
	}
	AActor* Owner = MeshComp->GetOwner();
	ensure(IsValid(Owner));

	if (Owner->HasAuthority())
	{
		if (true == bHitCheckOnTick)
		{
			if (false == IsValid(MeshComp))
			{
				return;
			}

			ADSCharacter* Character = Cast<ADSCharacter>(MeshComp->GetOwner());

			if (false == IsValid(Character))
			{
				return;
			}

			Character->AttackHitCheck(SkillType);
		}
	}
}

void UAnimNotifyState_AttackHitCheck::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

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

		Character->PostAttackHitCheck();
	}
}
