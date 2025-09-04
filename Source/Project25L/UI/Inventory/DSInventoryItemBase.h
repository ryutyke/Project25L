
#pragma once

// Default
#include "CoreMinimal.h"

// UE
#include "UI/Base/DSFocusedWidget.h"
#include "GameData/Items/DSItemData.h"

// UHT
#include "DSInventoryItemBase.generated.h"


class UImage;
class USizeBox;
class UCanvasPanelSlot;
class UBorder;


DECLARE_MULTICAST_DELEGATE_TwoParams(FOnItemDragged, const FDSItemInfo&, FIntPoint /*TopLeftIndex*/);
DECLARE_MULTICAST_DELEGATE_FiveParams(FOnItemTooltipRequested, const FDSItemInfo& /**ItemInfo*/, FIntPoint /*TopLeftIndex*/, EInventoryCategory /*Category*/, int32 /*TileSize*/, bool /*bIsVisible*/);



UCLASS()
class PROJECT25L_API UDSInventoryItemBase : public UDSFocusedWidget
{
	GENERATED_BODY()

public:
	FOnItemDragged OnItemDragged;
	FOnItemTooltipRequested OnItemTooltipRequested;

	// 마우스에 따라서 색 변경
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

	// 드래그 감지를 하기 위해 MouseButtonDown 함수 설정해줘야한다.
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	virtual void NativeConstruct() override;
	void InitializeItem(EInventoryCategory InInventoryCategory, const FDSItemInfo& InItemInfo, int32 InTileSize, FIntPoint InTopLeftIndex);
	void Refresh();
	void GetItemMaterial(bool bIsRotated);
	void NativeDestruct()override;


protected:

	UPROPERTY(Transient)
	EInventoryCategory InventoryCategory;

	UPROPERTY(Transient, BlueprintReadWrite)
	int32 TileSize;

	UPROPERTY(Transient, BlueprintReadWrite)
	FIntPoint TopLeftIndex;

	UPROPERTY(Transient, BlueprintReadWrite)
	FVector2D Size;

	UPROPERTY(BlueprintReadOnly)
	FDSItemInfo ItemInfo;


	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
	TObjectPtr<UImage> ItemImage;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
	TObjectPtr<UImage> ItemBorderImage;
	
	UPROPERTY(EditDefaultsOnly, Category = "DSSettings")
	TSubclassOf<UDSInventoryItemBase> DragVisualClass;
	
};
