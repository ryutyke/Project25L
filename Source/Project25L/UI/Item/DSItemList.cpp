// Default
#include "UI/Item/DSItemList.h"

// UE
#include "Components/CanvasPanel.h"

// Game
#include "Character/Characters/DSCharacter.h"
#include "GameData/Items/DSItemData.h"
#include "System/DSEventSystems.h"
#include "System/DSUIManagerSubsystem.h"
#include "UI/Item/DSItemEntry.h"
#include "UI/Item/DSItemTooltip.h"
#include "UI/Object/DSItemListView.h"

#include "DSLogChannels.h"

UDSItemList::UDSItemList(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, SelectedIdx(0)
{
}

void UDSItemList::NativeConstruct()
{
	Super::NativeConstruct();
	
	InitItemList();
	SetKeyboardFocus();
}

void UDSItemList::NativeDestruct()
{
	Super::NativeDestruct();

	if (IsValid(ListView_Items))
	{
		ListView_Items->ClearListItems();

		ListView_Items->OnListEntryHovered.Unbind();
		ListView_Items->OnListEntryUnhovered.Unbind();
	}
}

void UDSItemList::InitItemList()
{
	//캔버스에 있는 모든 ItemTooltip을 가지고온다.
	ItemTooltips.Empty();

	TArray<UWidget*> Widgets = Canvas_ItemTooltip->GetAllChildren();

	for (int WidgetIdx = 0; WidgetIdx < Widgets.Num(); WidgetIdx++)
	{
		UDSItemTooltip* ItemTooltip = Cast<UDSItemTooltip>(Widgets[WidgetIdx]);

		if (IsValid(ItemTooltip))
		{
			ItemTooltips.Add(ItemTooltip);

			ItemTooltip->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	if (IsValid(ListView_Items))
	{
		ListView_Items->OnListEntryHovered.BindUObject(this, &ThisClass::OnListEntryHovered);
		ListView_Items->OnListEntryUnhovered.BindUObject(this, &ThisClass::OnListEntryUnhovered);
	}
}
void UDSItemList::AddItems(TArray<FDSItemInfo>& ItemInfos)
{
	if (IsValid(ListView_Items))
	{
		//이미 그렸음.
		//이 부분 수정해야할 듯 . 예외의 경우를 만들지 말고,, 실제 동작할 때 델리게이트 연결을 적절히 바꿔야한다고 생각함
		//예를 들어서.. 연결 부분이 


		// if (ListView_Items->GetNumItems() != 0)
		// {
		// 	return;
		// }

		ListView_Items->ClearListItems();


		for (int DrawIdx = 0; DrawIdx < ItemInfos.Num(); DrawIdx++)
		{
			UDSItemUI* ItemUI = NewObject<UDSItemUI>(this);

			if (IsValid(ItemUI))
			{
				ItemUI->ID = ItemInfos[DrawIdx].ID;
				ListView_Items->AddItem(ItemUI);
			}

			if (ItemTooltips.IsValidIndex(DrawIdx))
			{
				//초반에 데이터를 리셋한다.
				ItemTooltips[DrawIdx]->SetText(ItemInfos[DrawIdx].ID);
			}
		}


		SelectedIdx = 0;
	}

}

void UDSItemList::RemoveItem(int32 IndexToRemove)
{

	if (IsValid(ListView_Items))
	{
		UObject* RemoveItem = ListView_Items->GetItemAt(IndexToRemove);
		
		if (IsValid(RemoveItem))
		{
			ListView_Items->RemoveItem(RemoveItem);
		}

		int32 MaxNum = ListView_Items->GetNumItems();
		int32 PreSelected = SelectedIdx;

		if (MaxNum > 0)
		{
			SelectedIdx = (SelectedIdx + 1) % MaxNum;
			OnListEntryUnhovered(PreSelected);
		}
	}
}

void UDSItemList::FocusOnItemEntry(int PreSelectedIdx)
{
	UDSItemUI* PreItemUI = Cast<UDSItemUI>(ListView_Items->GetItemAt(PreSelectedIdx));

	if (IsValid(PreItemUI))
	{
		UDSItemEntry* PreItemEntry = Cast<UDSItemEntry>(ListView_Items->GetEntryWidgetFromItem(PreItemUI));

		if (IsValid(PreItemEntry))
		{
			PreItemEntry->SetFocus(false);
		}
	}
	OnListEntryUnhovered(PreSelectedIdx);

	UDSItemUI* ItemUI = Cast<UDSItemUI>(ListView_Items->GetItemAt(SelectedIdx));

	if (IsValid(ItemUI))
	{
		UDSItemEntry* ItemEntry = Cast<UDSItemEntry>(ListView_Items->GetEntryWidgetFromItem(ItemUI));

		if (IsValid(ItemEntry))
		{
			ItemEntry->SetFocus(true);
		}
	}
	
	if (ItemTooltips.IsValidIndex(SelectedIdx))
	{
		ItemTooltips[SelectedIdx]->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}

	OnListEntryHovered(SelectedIdx);
	ListView_Items->SetSelectedIndex(SelectedIdx);
	ListView_Items->RequestRefresh();
}

void UDSItemList::OnListEntryHovered(int HoveredIdx)
{
	// 현재걸 업데이트
	if (ItemTooltips.IsValidIndex(HoveredIdx))
	{
		ItemTooltips[HoveredIdx]->SetVisibility(ESlateVisibility::HitTestInvisible);
	}

	SelectedIdx = HoveredIdx;
}

void UDSItemList::OnListEntryUnhovered(int UnHoveredIdx)
{
	// 이전걸 업데이트
	if (ItemTooltips.IsValidIndex(UnHoveredIdx))
	{
		ItemTooltips[UnHoveredIdx]->SetVisibility(ESlateVisibility::Collapsed);
	}
}

FReply UDSItemList::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	FReply Reply = Super::NativeOnKeyDown(InGeometry, InKeyEvent);

	if (InKeyEvent.GetKey() == EKeys::Escape)
	{
		SetVisibility(ESlateVisibility::Collapsed);
	}

	return Reply;
}