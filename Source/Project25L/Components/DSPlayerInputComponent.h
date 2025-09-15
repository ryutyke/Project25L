#pragma once
//Default
#include "CoreMinimal.h"

//UE
#include "Components/PawnComponent.h"
#include "GameplayTagContainer.h"

//Game
#include "GameData/DSEnums.h"
#include "Player/DSCheatManager.h"

//UHT
#include "DSPlayerInputComponent.generated.h"


struct FInputActionValue;
class UInputMappingContext;
class UDSInputConfig;
class ADSPlayerController;
class UDSInputComponent;
class UDSFlightComponent;

/** UDSPlayerInputComponent
 *
 * 플레이어 입력을 처리하고 InputAction과 GameplayTag를 매핑하는 Component
 */


UCLASS()
class PROJECT25L_API UDSPlayerInputComponent : public UPawnComponent
{
	GENERATED_BODY()
	friend class UDSCheatManager;

public:
	UDSPlayerInputComponent(const FObjectInitializer& ObjectInitializer);

	virtual void SetupInputComponent(UInputComponent* InputComponent);
	/*초기 InputContextMapping 부여*/
	void SetInputMappingContext(EInputMappingContextType NewIMCType);
	void OnInputMappingChanged(bool bCanFly, bool bIsCompleted);

	void InitialCharacterSetting();
	void SetCrounchMode(ECrouchMode TargetMode);

	ESpeedType GetSpeedType() { return CurrentSpeedType; }
protected:

	void SetSpeed(ESpeedType TargetwalkSpeed);

	UDSFlightComponent* GetFlightComponent() const;

protected:
	virtual void OnUnregister() override;

	// Move
	void Input_Move(const FInputActionValue& InputActionValue);
	void Input_Look_Mouse(const FInputActionValue& InputActionValue);
	void Input_Jump(const FInputActionValue& InputActionValue);
	void Input_StopJumping(const FInputActionValue& InputActionValue);
	void Input_Parkour(const FInputActionValue& InputActionValue);
	void Input_Sit(const FInputActionValue& InputActionValue);
	void Input_StopSit(const FInputActionValue& InputActionValue);
	void Input_ToggleSit(const FInputActionValue& InputActionValue);
	void Input_FastRun(const FInputActionValue& InputActionValue);
	void Input_StopFastRun(const FInputActionValue& InputActionValue);

	// System
	void Input_Pause(const FInputActionValue& InputActionValue);
	void Input_Interaction(const FInputActionValue& InputActionValue);

	// Item
	void Input_Item_QuickSlot1(const FInputActionValue& InputActionValue);
	void Input_Item_QuickSlot2(const FInputActionValue& InputActionValue);
	void Input_Item_QuickSlot3(const FInputActionValue& InputActionValue);
	void Input_Item_Vehicle(const FInputActionValue& InputActionValue);

	// Weapon
	void Input_Weapon_Toggle(const FInputActionValue& InputActionValue);

	virtual void Input_Weapon_Attack_Started(const FInputActionValue& InputActionValue, FGameplayTag InputTag);
	virtual void Input_Weapon_Attack_Onging(const FInputActionValue& InputActionValue, FGameplayTag InputTag);
	virtual void Input_Weapon_Attack_Completed(const FInputActionValue& InputActionValue, FGameplayTag InputTag);

	void Input_Equipment_Toggle();

	// UI
	void Input_UI_Inventory(const FInputActionValue& InputActionValue);
	void Input_UI_Status(const FInputActionValue& InputActionValue);

	// Flight
	void Input_Skill_Flight_Up(const FInputActionValue& InputActionValue); 
	void Input_Skill_Flight_Down(const FInputActionValue& InputActionValue);
	void Input_Skill_Flight_Begin(const FInputActionValue& InputActionValue);
	// 공통
	ESpeedType CalculateSpeed();

	void DefaultAttack(FGameplayTag InputTag);
	FGameplayTag ConvertInputTagToSkillTag(const FGameplayTag& InputTag);

protected:

	UPROPERTY(Transient)
	ESpeedType CurrentSpeedType;

	UPROPERTY(Transient)
	TObjectPtr<UDSInputComponent> DSInputComponent;

	UPROPERTY(Transient)
	uint8 bIsCrouched;

	UPROPERTY(Transient)
	uint8 bIsRun : 1;

	UPROPERTY(Transient)
	float PressedStartTime;

	UPROPERTY(Transient)
	float HoldTime;

	UPROPERTY(Transient)
	ECrouchMode CurrentCrouchMode;

	UPROPERTY(Transient)
	uint8 bIsInventoryMode;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DSSettings | Input")
	TMap <EInputMappingContextType, UInputMappingContext*> InputMappingContexts;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DSSettings | Input")
	TObjectPtr<UDSInputConfig> InputConfig;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DSSettings | Input")
	float MouseSensitivity;

};
