// Default
#include "UI/Menu/DSMenu.h"

// UE
#include "Components/Button.h"
#include "Components/VerticalBox.h"

// Game
#include "Player/DSLobbyPlayerController.h"
#include "UI/Menu/DSCharacterSelectionButton.h"

UDSMenu::UDSMenu(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, SelectedCharacterTypeIndex(-1)
	, CharacterTypes()
{
}

void UDSMenu::NativeConstruct()
{
	Super::NativeConstruct();

	if (IsValid(VBox_CharacterTypes))
	{
		TArray<UWidget*> CharacterTypeButtons = VBox_CharacterTypes->GetAllChildren();

		for (int TypeIdx = 0; TypeIdx < CharacterTypeButtons.Num(); TypeIdx++)
		{
			UDSCharacterSelectionButton* SelectionButton = Cast<UDSCharacterSelectionButton>(CharacterTypeButtons[TypeIdx]);

			if (IsValid(SelectionButton))
			{
				//OnClick 이벤트 연결
				SelectionButton->OnCharacterSelectionButtonClicked.AddUObject(this, &ThisClass::OnCharacterSelectionButtonClicked);
				CharacterTypes.Add(SelectionButton);
			}
		}
	}

	if (IsValid(But_StartButton))
	{
		But_StartButton->OnClicked.AddDynamic(this, &ThisClass::Ready);

	}
}

void UDSMenu::Ready()
{
	for (int TypeIdx = 0; TypeIdx < CharacterTypes.Num(); TypeIdx++)
	{
		CharacterTypes[TypeIdx]->SetIsEnabled(false);
	}

	ADSLobbyPlayerController* PlayerController = GetOwningPlayer<ADSLobbyPlayerController>();

	if (IsValid(PlayerController))
	{
		PlayerController->Ready();
	}
}

void UDSMenu::UpdateButtonStates(int32 CurrentButtonIdex, int32 PrevButtonIndex)
{
	if (CharacterTypes.IsValidIndex(CurrentButtonIdex))
	{
		CharacterTypes[CurrentButtonIdex]->SetIsEnabled(false);
	}

	if (CharacterTypes.IsValidIndex(PrevButtonIndex))
	{
		CharacterTypes[PrevButtonIndex]->SetIsEnabled(true);
	}
}

void UDSMenu::OnCharacterSelectionButtonClicked(ECharacterType CharacterType)
{
	int32 SelectedButtonIndex = 0;

	for (int TypeIdx = 0; TypeIdx < CharacterTypes.Num(); TypeIdx++)
	{
		if (CharacterTypes[TypeIdx]->GetCharacterType() == CharacterType)
		{
			SelectedButtonIndex = TypeIdx;
			break;
		}
	}

	ADSLobbyPlayerController* PlayerController = GetOwningPlayer<ADSLobbyPlayerController>();

	if (IsValid(PlayerController))
	{
		PlayerController->SelectedCharacterType(CharacterType, SelectedButtonIndex, SelectedCharacterTypeIndex);
	}

	//현재 선택한 버튼, 이전에 선택한 버튼
	UpdateButtonStates(SelectedButtonIndex, SelectedCharacterTypeIndex);

	SelectedCharacterTypeIndex = SelectedButtonIndex;
}