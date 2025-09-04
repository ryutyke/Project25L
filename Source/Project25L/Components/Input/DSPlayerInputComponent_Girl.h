#pragma once
// Default
#include "CoreMinimal.h"

// Game
#include "Components/DSPlayerInputComponent.h"

// UHT
#include "DSPlayerInputComponent_Girl.generated.h"

UCLASS()
class PROJECT25L_API UDSPlayerInputComponent_Girl : public UDSPlayerInputComponent
{
	GENERATED_BODY()
	
public:
	UDSPlayerInputComponent_Girl(const FObjectInitializer& ObjectInitializer);

	DECLARE_MULTICAST_DELEGATE_OneParam(FOnMouseWheelScrolled, float);
	FOnMouseWheelScrolled OnMouseWheelScrolled;

protected:
	virtual void SetupInputComponent(UInputComponent* InputComponent) override;

protected:
	virtual void Input_Weapon_Attack_Started(const FInputActionValue& InputActionValue, FGameplayTag InputTag);
	virtual void Input_Weapon_Attack_Onging(const FInputActionValue& InputActionValue, FGameplayTag InputTag);
	virtual void Input_Weapon_Attack_Completed(const FInputActionValue& InputActionValue, FGameplayTag InputTag);

protected:
	void Input_Weapon_Attack_Started_Child(const FInputActionValue& InputActionValue, FGameplayTag InputTag);
	void Input_Weapon_Attack_Completed_Child(const FInputActionValue& InputActionValue, FGameplayTag InputTag);

	void Input_Zoom(const FInputActionValue& InputActionValue);

protected:
	UPROPERTY(Transient)
	FGameplayTag ActivatedSkill;
};
