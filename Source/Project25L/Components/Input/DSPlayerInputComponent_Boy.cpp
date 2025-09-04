// Default
#include "Components/Input/DSPlayerInputComponent_Boy.h"

// Game
#include "Character/DSArmedCharacter.h"
#include "GameData/GameplayTag/DSGameplayTags.h"

void UDSPlayerInputComponent_Boy::Input_Weapon_Attack_Started(const FInputActionValue& InputActionValue, FGameplayTag InputTag)
{
	ADSCharacter* Character = GetPawn<ADSCharacter>();

	if (IsValid(Character))
	{
		ADSArmedCharacter* ArmedCharacter = Cast<ADSArmedCharacter>(Character);

		if (IsValid(ArmedCharacter))
		{
			bool bIsEquipped = ArmedCharacter->GetIsEquipped();

			if (bIsEquipped == false)
			{
				ArmedCharacter->PlayWeaponActionMontage(EWeaponState::Equipped);

				return;
			}

			// HoldTime = Character->GetInputThreshold(InputTag);

			FGameplayTag SkillTag = ConvertInputTagToSkillTag(InputTag);
			if (SkillTag == FDSTags::GetDSTags().Skill_None)
			{
				return;
			}
			DefaultAttack(SkillTag);

			if (InputTag == FDSTags::GetDSTags().InputTag_Skill_PrimarySkill)
			{
				// 평타일 경우 콤보 true
				//Character->SetNextComboCommand(true);
				Character->ServerRPC_SetNextComboCommand(true);
			}
		}
	}
}

void UDSPlayerInputComponent_Boy::Input_Weapon_Attack_Onging(const FInputActionValue& InputActionValue, FGameplayTag InputTag)
{
	
}

void UDSPlayerInputComponent_Boy::Input_Weapon_Attack_Completed(const FInputActionValue& InputActionValue, FGameplayTag InputTag)
{
	ADSCharacter* Character = GetPawn<ADSCharacter>();

	if (IsValid(Character))
	{
		if (InputTag == FDSTags::GetDSTags().InputTag_Skill_PrimarySkill)
		{
			// 평타일 경우 콤보 false
			//Character->SetNextComboCommand(false);
			Character->ServerRPC_SetNextComboCommand(false);
		}
	}
}
