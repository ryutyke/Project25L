// Default
#include "UI/Inventory/Combat/DSPlayerSendPanel.h"

// UE
#include "components/Image.h"
#include "components/CanvasPanelSlot.h"
#include "Components/TextBlock.h"
#include "GameFramework/PlayerState.h"

// Game
#include "System/DSGameDataSubsystem.h"
#include "Character/Characters/DSCharacter.h"
#include "UI/Inventory/DSItemDragDropOperation.h"
#include "DSLogChannels.h"
#include "System/DSGameUtils.h"

void UDSPlayerSendPanel::NativeConstruct()
{
	Super::NativeConstruct();

	if (IsValid(SendImage))
	{
		SetFocus(SendImage, false);
	}
}

bool UDSPlayerSendPanel::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	bool bSuperHandled = Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
	UWorld* World = GetWorld();
	check(World);

	const UDSItemDragDropOperation* ItemDragDrop = Cast<UDSItemDragDropOperation>(InOperation);
	if (false == IsValid(ItemDragDrop))
	{
		return true;
	}

	ADSCharacter* Character = Cast<ADSCharacter>(UDSGameUtils::GetCharacter(GetOwningPlayer()));

	if (IsValid(Character))
	{	
		Character->SendItem(PlayerId, ItemDragDrop->ItemInfo.ID);
	}
	return true;
}

void UDSPlayerSendPanel::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	SetFocus(SendImage, true);
}

void UDSPlayerSendPanel::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	SetFocus(SendImage, false);
}

void UDSPlayerSendPanel::SetPlayerId(int32 ID)
{
	PlayerId = ID;
	FString IDString = FString::Printf(TEXT("%d"), ID);
	Text_PlayerName->SetText(FText::FromString(IDString));
}

