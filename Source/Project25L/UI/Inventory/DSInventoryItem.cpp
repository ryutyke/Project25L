// Default
#include "UI/Inventory/DSInventoryItem.h"

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


FReply UDSInventoryItem::NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		DSEVENT_DELEGATE_INVOKE(OnItemUse, ItemInfo, TopLeftIndex)
		return FReply::Handled();
	}

	return FReply::Unhandled();
}


