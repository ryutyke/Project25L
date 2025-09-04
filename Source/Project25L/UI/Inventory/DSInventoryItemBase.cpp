
// Default
#include "UI/Inventory/DSInventoryItemBase.h"

// UE
#include "Components/SizeBox.h"
#include "components/Image.h"
#include "components/CanvasPanelSlot.h"
#include "Materials/MaterialInterface.h"
#include "components/Border.h"
#include "Blueprint/DragDropOperation.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Framework/Application/SlateApplication.h"
#include "InputCoreTypes.h"


// Game
#include "System/DSGameDataSubsystem.h"
#include "DSLogChannels.h"
#include "UI/Inventory/DSItemDragDropOperation.h"
#include "Components/DSInventoryComponent.h"


void UDSInventoryItemBase::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

	SetFocus(ItemBorderImage, true);

	DSEVENT_DELEGATE_INVOKE(OnItemTooltipRequested, ItemInfo, TopLeftIndex, InventoryCategory, TileSize, true);
}

void UDSInventoryItemBase::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
	SetFocus(ItemBorderImage, false);
	DSEVENT_DELEGATE_INVOKE(OnItemTooltipRequested, ItemInfo, TopLeftIndex, InventoryCategory, TileSize, false);

}

FReply UDSInventoryItemBase::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Reply = Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		return Reply.Handled().DetectDrag(TakeWidget(), EKeys::LeftMouseButton);
	}

	return Reply.Unhandled();
}

void UDSInventoryItemBase::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);
	DSEVENT_DELEGATE_INVOKE(OnItemTooltipRequested, ItemInfo, TopLeftIndex, InventoryCategory, TileSize, false);
	if (IsValid(DragVisualClass))
	{
		UDSInventoryItemBase* DragVisual = CreateWidget<UDSInventoryItemBase>(this, DragVisualClass);
		ensure(DragVisual != nullptr);
		DragVisual->InitializeItem(InventoryCategory, ItemInfo, TileSize, TopLeftIndex);
		DragVisual->SetRenderOpacity(0.5f);
		// 드래그 오퍼레이션 생성
		UDSItemDragDropOperation* DragOp = NewObject<UDSItemDragDropOperation>();

		// 아이템 정보 저장
		DragOp->ItemInfo = ItemInfo;
		DragOp->TopLeftIndex = TopLeftIndex;
		DragOp->InventoryCategory = InventoryCategory;
		DragOp->DefaultDragVisual = DragVisual;
		DragOp->Pivot = EDragPivot::CenterCenter;
		OutOperation = DragOp;
	}

	DSEVENT_DELEGATE_INVOKE(OnItemDragged, ItemInfo, TopLeftIndex)

	RemoveFromParent();
}

void UDSInventoryItemBase::NativeConstruct()
{
	Super::NativeConstruct();
	Refresh();
}

void UDSInventoryItemBase::InitializeItem(EInventoryCategory InInventoryCategory, const FDSItemInfo& InItemInfo, int32 InTileSize, FIntPoint InTopLeftIndex)
{
	ItemInfo = InItemInfo;
	TileSize = InTileSize;
	TopLeftIndex = InTopLeftIndex;
	InventoryCategory = InInventoryCategory;
	GetItemMaterial(InItemInfo.isRotated);
}

void UDSInventoryItemBase::Refresh()
{
	UDSGameDataSubsystem* DataManager = UDSGameDataSubsystem::Get(this);

	check(DataManager);

	FDSItemData* ItemData = static_cast<FDSItemData*>(DataManager->GetDataRowByID(EDataTableType::ItemData, ItemInfo.ID));

	const FIntPoint Dimensions = ItemData->Dimensions;
	const bool bIsRotated = ItemInfo.isRotated;
	const FIntPoint FinalSize = bIsRotated ? FIntPoint(Dimensions.Y, Dimensions.X) : Dimensions;

	// 사이즈 설정
	Size.X = FinalSize.X * TileSize;
	Size.Y = FinalSize.Y * TileSize;

	if (IsValid(ItemImage) && IsValid(ItemBorderImage))
	{
		UCanvasPanelSlot* CanvasItemSlot = Cast<UCanvasPanelSlot>(ItemImage->Slot);
		if (IsValid(CanvasItemSlot))
		{
			CanvasItemSlot->SetSize(Size);
		}
		UCanvasPanelSlot* CanvasItemBorderSlot = Cast<UCanvasPanelSlot>(ItemBorderImage->Slot);
		if (IsValid(CanvasItemBorderSlot))
		{
			CanvasItemBorderSlot->SetSize(FVector2D(Size.X + 10, Size.Y + 10));
			CanvasItemBorderSlot->SetPosition(FVector2D(-5.f, -5.f));
		}
		SetFocus(ItemBorderImage, false);
	}
}

void UDSInventoryItemBase::GetItemMaterial(bool bIsRotated)
{
	UDSGameDataSubsystem* DataManager = UDSGameDataSubsystem::Get(this);

	check(DataManager);

	FDSItemData* ItemData = static_cast<FDSItemData*>(DataManager->GetDataRowByID(EDataTableType::ItemData, ItemInfo.ID));

	UMaterialInterface* Material = nullptr;
	if (bIsRotated)
	{
		Material = ItemData->ItemTileMaterialRotated.LoadSynchronous();
	}
	else
	{
		Material = ItemData->ItemTileMaterial.LoadSynchronous();
	}

	if (false == IsValid(Material))
	{
		return;
	}

	FSlateBrush Brush;
	Brush.SetResourceObject(Material);
	Brush.ImageSize = FVector2D(Size.X, Size.Y);
	Brush.DrawAs = ESlateBrushDrawType::Image;
	ItemImage->SetBrush(Brush);
}

void UDSInventoryItemBase::NativeDestruct()
{
	Super::NativeDestruct();
	DSEVENT_DELEGATE_INVOKE(OnItemTooltipRequested, ItemInfo, TopLeftIndex, InventoryCategory, TileSize, false);
}

