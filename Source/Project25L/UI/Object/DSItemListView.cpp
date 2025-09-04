//Default
#include "UI/Object/DSItemListView.h"

// Game
#include "System/DSUIManagerSubsystem.h"
#include "Character/Characters/DSCharacter.h"
#include "DSLogChannels.h"
#include "UI/Item/DSItemEntry.h"

void UDSItemListView::HandleListEntryHovered(UUserWidget& EntryWidget)
{
	Super::HandleListEntryHovered(EntryWidget);

	// 현재 포커스되어 있는 Widget을 가져온다.
	UDSItemEntry* ItemEntry = Cast<UDSItemEntry>(&EntryWidget);

	if (IsValid(ItemEntry))
	{
		// 현재 EntryWidget을 킨다.
		ItemEntry->SetFocus(true);
		
		int32 SelectedIdx = GetIndexForItem(ItemEntry->GetListItem());

		OnListEntryHovered.ExecuteIfBound(SelectedIdx);
	}

}

void UDSItemListView::HandleListEntryUnhovered(UUserWidget& EntryWidget)
{
	Super::HandleListEntryUnhovered(EntryWidget);
	//이전 위젯 포커스를 끊어낸다.
	UDSItemEntry* ItemEntry = Cast<UDSItemEntry>(&EntryWidget);

	if (IsValid(ItemEntry))
	{
		// 현재 EntryWidget을 끈다.
		ItemEntry->SetFocus(false);

		int32 SelectedIdx = GetIndexForItem(ItemEntry->GetListItem());

		OnListEntryUnhovered.ExecuteIfBound(SelectedIdx);
	}
}

void UDSItemListView::OnItemClickedInternal(UObject* Item)
{
	Super::OnItemClickedInternal(Item);
	
	int32 SelectedIdx = GetIndexForItem(Item);

	APlayerController* PlayerController = GetOwningPlayer();

	if (false == IsValid(PlayerController))
	{
		return;
	}

	ADSCharacter* Character = PlayerController->GetPawn<ADSCharacter>();

	if (IsValid(Character))
	{
		Character->TryPickupItem(SelectedIdx);
	}
}
