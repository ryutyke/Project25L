// Default
#include "Components/Skill/DSSkillControlComponent_Mister.h"

// UE

// Game
#include "Character/Characters/DSCharacter.h"
#include "DSLogChannels.h"

void UDSSkillControlComponent_Mister::AttackPrimarySkill(UAnimMontage* AttackAnimMontage,const FName& NextSection)
{
	PlayMontage(AttackAnimMontage, NextSection);

	ServerRPC_AttackMontage(AttackAnimMontage, NextSection);
}

void UDSSkillControlComponent_Mister::ServerRPC_AttackMontage_Implementation(UAnimMontage* AttackAnimMontage, const FName& NextSection)
{

	PlayMontage(AttackAnimMontage, NextSection);

	UWorld* World = GetWorld();
	check(World);

	//모든 클라이언트를 가지고 온다.
	for (FConstPlayerControllerIterator Iterator = World->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* PlayerController = Iterator->Get();

		if (IsValid(PlayerController) && GetController<APlayerController>() != PlayerController)
		{
			if (false == PlayerController->IsLocalController())
			{
				ADSCharacterBase* OtherPlayer = Cast<ADSCharacterBase>(PlayerController->GetPawn());
				if (IsValid(OtherPlayer))
				{
					UDSSkillControlComponent_Mister* SkillControlComponent = Cast<UDSSkillControlComponent_Mister>(OtherPlayer->GetSkillControlComponent());
					
					if (IsValid(SkillControlComponent))
					{
						SkillControlComponent->ClientRPC_AttackMontage(GetPawn<ADSCharacterBase>(), AttackAnimMontage, NextSection);
					}
				}
			}
		}
	}

}

void UDSSkillControlComponent_Mister::ClientRPC_AttackMontage_Implementation(ADSCharacterBase* OtherPlayer,UAnimMontage* AnimMontage, const FName& NextSection)
{
	if (IsValid(OtherPlayer))
	{
		UDSSkillControlComponent_Mister* SkillControlComponent = Cast<UDSSkillControlComponent_Mister>(OtherPlayer->GetSkillControlComponent());

		if (IsValid(SkillControlComponent))
		{
			SkillControlComponent->PlayMontage(AnimMontage, NextSection);
		}
	}
}

void UDSSkillControlComponent_Mister::PlayMontage(UAnimMontage* AnimMontage, const FName& NextSection)
{
	if (false == IsValid(AnimMontage))
	{
		return;
	}
	ADSCharacterBase* Character = GetPawn<ADSCharacterBase>();

	if (false == IsValid(Character))
	{
		return;
	}
	USkeletalMeshComponent* SkeletalMesh = Character->GetMesh();
	if (false == IsValid(SkeletalMesh))
	{
		return;
	}
	UAnimInstance* AnimInstance = SkeletalMesh->GetAnimInstance();

	if (false == IsValid(AnimInstance))
	{
		return;
	}

	if (false == AnimInstance->Montage_IsPlaying(AnimMontage) || NextSection.IsNone())
	{
		//첫 실행
		Character->PlayAnimMontage(AnimMontage);
	}
	else
	{
		AnimInstance->Montage_JumpToSection(NextSection, AnimMontage);
	}
}