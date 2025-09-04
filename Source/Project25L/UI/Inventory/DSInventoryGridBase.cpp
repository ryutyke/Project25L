// Default
#include "UI/Inventory/DSInventoryGridBase.h"

// UE
#include "Blueprint/SlateBlueprintLibrary.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Border.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/PanelWidget.h"

// Game
#include "Components/DSInventoryComponent.h"
#include "DSLogChannels.h"
#include "Item/DSItem.h"
#include "System/DSEventSystems.h"
#include "System/DSGameDataSubsystem.h"
#include "System/DSSpawnerSubsystem.h"
#include "UI/Inventory/DSInventoryItem.h"
#include "UI/Inventory/DSItemDragDropOperation.h"
#include "UI/Inventory/Widget/DSCombatInventoryWidget.h"
#include "Character/Characters/DSCharacter.h"

/*
*	함수 호출 순서
* // InitializeWidget/Init -> BindEvents
*/

UDSInventoryGridBase::UDSInventoryGridBase(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
	, InventoryComponent(nullptr)
{
	InventoryInfo.TileSize = 0;
}

void UDSInventoryGridBase::Init(EInventoryCategory InInventoryCategory, const FInventoryInfo& InInventoryInfo)
{
	InventoryCategory = InInventoryCategory;
	InventoryInfo = InInventoryInfo;
	TileSize = InventoryInfo.TileSize;

	CreateLineSegments();
}

bool UDSInventoryGridBase::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	bool bSuperHandled = Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

	const UDSItemDragDropOperation* ItemDragDrop = Cast<UDSItemDragDropOperation>(InOperation);

	if (false == IsValid(ItemDragDrop))
	{
		return false;
	}

	// 그 위치에 드래그 놓기
	int TopLeftIndex = InventoryComponent->TiletoIndex(ItemDragDrop->TopLeftIndex, InventoryInfo.Columns);

	if (InventoryComponent->IsRoomAvailable(ItemDragDrop->ItemInfo, TopLeftIndex))
	{
		InventoryComponent->AddItemAtFromGrid(ItemDragDrop->ItemInfo, TopLeftIndex);
		return true;
	}

	return true;
}

bool UDSInventoryGridBase::NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	bool bSuperHandled = Super::NativeOnDragOver(InGeometry, InDragDropEvent, InOperation);

	FVector2D ScreenPosition = InDragDropEvent.GetScreenSpacePosition();
	FVector2D LocalPosition = InGeometry.AbsoluteToLocal(ScreenPosition);

	UDSItemDragDropOperation* ItemDragDrop = Cast<UDSItemDragDropOperation>(InOperation);
	if (false == IsValid(ItemDragDrop))
	{
		return bSuperHandled;
	}

	UDSGameDataSubsystem* DataManager = UDSGameDataSubsystem::Get(this);
	check(DataManager);

	FDSItemData* ItemData = static_cast<FDSItemData*>(DataManager->GetDataRowByID(EDataTableType::ItemData, ItemDragDrop->ItemInfo.ID));
	
	if (nullptr == ItemData)
	{
		return bSuperHandled;
	}

	const FIntPoint Dimensions = ItemData->Dimensions;
	const bool bIsRotated = ItemDragDrop->ItemInfo.isRotated;
	const FIntPoint FinalSize = bIsRotated ? FIntPoint(Dimensions.Y, Dimensions.X) : Dimensions;


	const int32 TileX = FMath::TruncToInt(LocalPosition.X / InventoryInfo.TileSize);
	const int32 TileY = FMath::TruncToInt(LocalPosition.Y / InventoryInfo.TileSize);

	FIntPoint Tile(TileX, TileY);

	const FIntPoint Direction = MousePositionInIntile(LocalPosition, bIsRotated);

	const int32 OffsetX = (FinalSize.X % 2 == 0) ? (1 - Direction.X) : FinalSize.X / 2;
	const int32 OffsetY = (FinalSize.Y % 2 == 0) ? (1 - Direction.Y) : FinalSize.Y / 2;

	int32 TopLeftX = Tile.X - OffsetX;
	int32 TopLeftY = Tile.Y - OffsetY;

	const int32 MaxColumns = InventoryInfo.Columns;
	const int32 MaxRows = InventoryInfo.Rows;

	TopLeftX = FMath::Clamp(TopLeftX, 0, MaxColumns - FinalSize.X);
	TopLeftY = FMath::Clamp(TopLeftY, 0, MaxRows - FinalSize.Y);

	ItemDragDrop->TopLeftIndex = FIntPoint(TopLeftX, TopLeftY);

	return true;
}

FReply UDSInventoryGridBase::NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == EKeys::R)
	{
		UDragDropOperation* DragOp = UWidgetBlueprintLibrary::GetDragDroppingContent();
		UDSItemDragDropOperation* ItemDragDrop = Cast<UDSItemDragDropOperation>(DragOp);
		if (false == IsValid(ItemDragDrop))
		{
			return FReply::Unhandled();
		}
		UDSInventoryItem* DragVisual = Cast<UDSInventoryItem>(ItemDragDrop->DefaultDragVisual);
		if (false == IsValid(DragVisual))
		{
			return FReply::Unhandled();
		}

		// 회전 토글
		ItemDragDrop->ItemInfo.isRotated = !ItemDragDrop->ItemInfo.isRotated;

		// DragVisual 초기화 
		DragVisual->InitializeItem(InventoryCategory, ItemDragDrop->ItemInfo, InventoryInfo.TileSize, ItemDragDrop->TopLeftIndex);
		DragVisual->SetRenderOpacity(0.5f);
		return FReply::Handled();
	}

	return Super::NativeOnPreviewKeyDown(InGeometry, InKeyEvent);
}

void UDSInventoryGridBase::BindEvents()
{
	
	/*
		InitializeWidget보다 빨리 불려서 Pawn이 없음-> InventoryComponent가 없음
		따라서 임의로 EventBind하는 부분은 InitializeWidget 함수로 이동
	*/
	
}

void UDSInventoryGridBase::InitializeWidget()
{
	ADSCharacter* Character = Cast<ADSCharacter>(GetOwningPlayerPawn());
	if(false == IsValid(Character))
	{
		return;
	}
	
	InventoryComponent = Character->GetInventoryComponent();

	if(true == IsValid(InventoryComponent))
	{
		InventoryComponent->OnInventorySlotUpdated.Add(InventoryCategory, FOnInventorySlotUpdated());
		DSEVENT_DELEGATE_BIND(InventoryComponent->OnInventorySlotUpdated[InventoryCategory], this, &UDSInventoryGridBase::Refresh);
	}
}

void UDSInventoryGridBase::CreateLineSegments()
{
	Lines.Empty();
	for (int X = 0; X <= InventoryInfo.Columns; ++X)
	{
		int XPos = X * InventoryInfo.TileSize;

		FDSLine Line(XPos, 0, XPos, InventoryInfo.Rows * InventoryInfo.TileSize);
		Lines.Add(Line);
	}

	for (int Y = 0; Y <= InventoryInfo.Rows; ++Y)
	{
		int YPos = Y * InventoryInfo.TileSize;

		FDSLine Line(0, YPos, InventoryInfo.Columns * InventoryInfo.TileSize, YPos);
		Lines.Add(Line);
	}
}

void UDSInventoryGridBase::Refresh(const FDSItemInfo& ItemInfo, FIntPoint TopLeftIndex, bool bRemoved)
{
	if (bRemoved)
	{
		RemoveItemWidgetFromSlot(TopLeftIndex);
	}
	else
	{
		AddItemWidgetToSlot(ItemInfo, TopLeftIndex);
	}
}

UDSInventoryItemBase* UDSInventoryGridBase::AddItemWidgetToSlot(const FDSItemInfo& ItemInfo, FIntPoint TopLeftIndex)
{
	APlayerController* OwningPlayer = GetOwningPlayer();
	if (false == IsValid(OwningPlayer))
	{
		return nullptr;
	}

	UDSInventoryItemBase* ItemWidget = CreateWidget<UDSInventoryItemBase>(OwningPlayer, InventoryItemClass);
	ensure(ItemWidget != nullptr);

	if (false == IsValid(ItemWidget))
	{
		return nullptr;
	}

	ItemWidget->InitializeItem(InventoryCategory, ItemInfo, InventoryInfo.TileSize, TopLeftIndex);
	DSEVENT_DELEGATE_BIND(ItemWidget->OnItemDragged, InventoryComponent.Get(), &UDSInventoryComponent::RemoveItem);

	UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(GridCanvasPanel->AddChild(ItemWidget));
	if (false == IsValid(CanvasSlot))
	{
		CanvasSlot->SetAutoSize(true);
		CanvasSlot->SetPosition(FVector2D(TopLeftIndex.X * InventoryInfo.TileSize, TopLeftIndex.Y * InventoryInfo.TileSize));
	}

	return ItemWidget;

}

void UDSInventoryGridBase::RemoveItemWidgetFromSlot(FIntPoint TopLeftIndex)
{
	for (UWidget* ChildWidget : GridCanvasPanel->GetAllChildren())
	{
		UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(ChildWidget->Slot);
		if (IsValid(CanvasSlot))
		{
			FVector2D ItemPosition = CanvasSlot->GetPosition();
			if (ItemPosition.X == TopLeftIndex.X * InventoryInfo.TileSize && ItemPosition.Y == TopLeftIndex.Y * InventoryInfo.TileSize)
			{
				UDSInventoryItem* InventoryItem = Cast<UDSInventoryItem>(ChildWidget);
				if (IsValid(InventoryItem))
				{
					ChildWidget->RemoveFromParent();
					return;
				}

			}
		}
	}
}

FIntPoint UDSInventoryGridBase::MousePositionInIntile(FVector2D MousePosition, bool bIsRotated)
{
	// 00 : 쇱そ 	// 10 : ㅻ뒛履	// 01 : 쇱そ 꾨옒
	// 11 : ㅻ뒛履꾨옒
	const float HalfTile = InventoryInfo.TileSize / 2.f;

	const float ModX = FMath::Fmod(MousePosition.X, InventoryInfo.TileSize);
	const float ModY = FMath::Fmod(MousePosition.Y, InventoryInfo.TileSize);

	int32 Right = (ModX > HalfTile) ? 1 : 0;
	int32 Down = (ModY > HalfTile) ? 1 : 0;

	if (bIsRotated)
	{
		return FIntPoint(Down, Right);
	}
	return FIntPoint(Right, Down);
}
