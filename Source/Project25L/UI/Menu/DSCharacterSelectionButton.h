
#pragma once

// Default
#include "CoreMinimal.h"

// UE
#include "Components/Button.h"

// Game
#include "GameData/DSEnums.h"

// UHT
#include "DSCharacterSelectionButton.generated.h"

UCLASS()
class PROJECT25L_API UDSCharacterSelectionButton : public UButton
{
	GENERATED_BODY()

public:
	UDSCharacterSelectionButton(const FObjectInitializer& ObjectInitializer);

	ECharacterType GetCharacterType() { return CharacterType; }
	
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnCharacterSelectionButtonClicked, ECharacterType);
	FOnCharacterSelectionButtonClicked OnCharacterSelectionButtonClicked;

	UFUNCTION(BlueprintCallable)
	void ClickSelectionButton();
protected:

	UPROPERTY(EditAnywhere, Category = "DSSettings | CharacterType")
	ECharacterType CharacterType;
};
