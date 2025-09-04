#pragma once

// Default
#include "CoreMinimal.h"

// Game
#include "Components/DSPlayerInputComponent.h"

// UHT
#include "DSPlayerInputComponent_Boy.generated.h"

UCLASS()
class PROJECT25L_API UDSPlayerInputComponent_Boy : public UDSPlayerInputComponent
{
	GENERATED_BODY()

protected:
	virtual void Input_Weapon_Attack_Started(const FInputActionValue& InputActionValue, FGameplayTag InputTag) override;
	virtual void Input_Weapon_Attack_Onging(const FInputActionValue& InputActionValue, FGameplayTag InputTag) override;
	virtual void Input_Weapon_Attack_Completed(const FInputActionValue& InputActionValue, FGameplayTag InputTag) override;
};
