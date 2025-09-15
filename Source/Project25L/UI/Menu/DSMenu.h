#pragma once

// Default
#include "CoreMinimal.h"

// Game
#include "UI/Base/DSUserWidget.h"

// UHT
#include "DSMenu.generated.h"

class UDSCharacterSelectionButton;
class UVerticalBox;
class UButton;

UCLASS()
class PROJECT25L_API UDSMenu : public UDSUserWidget
{
	GENERATED_BODY()

public:

	UDSMenu(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct() override;

	void UpdateButtonStates(int32 CurrentButtonIdex, int32 PrevButtonIndex);

	//레디를 누르면 플레이어 캐릭터를 못누르도록 다 막힘
	
protected:

	UFUNCTION()
	void OnCharacterSelectionButtonClicked(ECharacterType CharacterType);

	UFUNCTION()
	void Ready();
protected:
	UPROPERTY(Transient)
	int32 SelectedCharacterTypeIndex;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UDSCharacterSelectionButton>> CharacterTypes;
	
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> But_StartButton;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UVerticalBox> VBox_CharacterTypes;
};
