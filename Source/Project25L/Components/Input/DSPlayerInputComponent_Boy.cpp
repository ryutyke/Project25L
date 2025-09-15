// Default
#include "Components/Input/DSPlayerInputComponent_Boy.h"

// Game
#include "Character/DSArmedCharacter.h"
#include "GameData/GameplayTag/DSGameplayTags.h"
#include "DSLogChannels.h"

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

			FGameplayTag SkillTag = ConvertInputTagToSkillTag(InputTag);
			if (SkillTag == FDSTags::GetDSTags().Skill_None)
			{
				return;
			}
			DefaultAttack(SkillTag);

			if (InputTag == FDSTags::GetDSTags().InputTag_Skill_PrimarySkill)
			{
				if (false == Character->GetNextComboCommand() && Character->IsComboInputTiming())
				{
					Character->SetNextComboCommand(true);

					if (false == HasAuthority())
					{
						Character->ServerRPC_SetNextComboCommand(true);
					}
				}
			}
		}
	}
}

void UDSPlayerInputComponent_Boy::Input_Weapon_Attack_Onging(const FInputActionValue& InputActionValue, FGameplayTag InputTag)
{
	// Super 호출 x
}

void UDSPlayerInputComponent_Boy::Input_Weapon_Attack_Completed(const FInputActionValue& InputActionValue, FGameplayTag InputTag)
{
	// Super 호출 x
}
