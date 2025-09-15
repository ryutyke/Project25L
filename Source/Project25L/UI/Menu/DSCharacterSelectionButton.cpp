// Default
#include "UI/Menu/DSCharacterSelectionButton.h"

// Game
#include "System/DSEventSystems.h"

#include "DSLogChannels.h"
UDSCharacterSelectionButton::UDSCharacterSelectionButton(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}


void UDSCharacterSelectionButton::ClickSelectionButton()
{
	DSEVENT_DELEGATE_INVOKE(OnCharacterSelectionButtonClicked, CharacterType);
}
