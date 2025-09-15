#pragma once

// Default
#include "CoreMinimal.h"

// UE
#include "Blueprint/UserWidget.h"

// Game
#include "GameData/Items/DSItemData.h"
#include "UI/Inventory/DSInventoryItemBase.h"
#include "UI/Base/DSFocusedWidget.h"

// UHT
#include "DSInventoryItem.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnItemUse, const FDSItemInfo& /**ItemInfo*/, FIntPoint /*TopLeftIndex*/);


UCLASS()
class PROJECT25L_API UDSInventoryItem : public UDSInventoryItemBase
{
	GENERATED_BODY()

public:
	FOnItemUse OnItemUse;

	virtual FReply NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

protected:


};
