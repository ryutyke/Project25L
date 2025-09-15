#pragma once

// Default
#include "CoreMinimal.h"

// Game
#include "UI/Inventory/DSInventoryGridBase.h"
#include "GameData/Items/DSItemData.h"

// UHT
#include "DSInventoryGridWidget.generated.h"


class UDSInventoryItemBase;

UCLASS()
class PROJECT25L_API UDSInventoryGridWidget : public UDSInventoryGridBase
{
	GENERATED_BODY()
	
public:
	UDSInventoryGridWidget(const FObjectInitializer& ObjectInitializer);
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual UDSInventoryItemBase* AddItemWidgetToSlot(const FDSItemInfo& ItemInfo, FIntPoint TopLeftIndex) override;

private:
	EItemType ItemType;

};
