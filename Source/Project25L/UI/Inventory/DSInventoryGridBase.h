#pragma once

// Default
#include "CoreMinimal.h"

// UE
#include "Blueprint/UserWidget.h"

// Game
#include "UI/Base/DSUserWidget.h"
#include "GameData/Items/DSItemData.h"

// UHT
#include "DSInventoryGridBase.generated.h"

class UDSInventoryItemBase;
class UDSInventoryComponent;
class UDSInventoryItem;
class UPanelWidget;
class UCanvasPanel;
class UBorder;

UCLASS()
class PROJECT25L_API UDSInventoryGridBase : public UDSUserWidget
{
	GENERATED_BODY()

public:
	UDSInventoryGridBase(const FObjectInitializer& ObjectInitializer);
	
	void Init(EInventoryCategory InInventoryCategory, const FInventoryInfo& InInventoryInfo);
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual bool NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual FReply NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

	virtual void BindEvents() override;
	virtual void InitializeWidget() override;

	// 선 만들어 주는 함수
	void CreateLineSegments();

	void Refresh(const FDSItemInfo& ItemInfo, FIntPoint TopLeftIndex, bool bRemoved);
	virtual UDSInventoryItemBase* AddItemWidgetToSlot(const FDSItemInfo& ItemInfo, FIntPoint TopLeftIndex);
	void RemoveItemWidgetFromSlot(FIntPoint TopLeftIndex);


	UFUNCTION()
	FIntPoint  MousePositionInIntile(FVector2D MousePosition, bool bIsRotated);


protected:
	UPROPERTY(Transient)
	EInventoryCategory InventoryCategory;

	UPROPERTY(Transient)
	TObjectPtr<UDSInventoryComponent> InventoryComponent;

	/* Inventory 텍스처 받으면 변수 지워주기 */
	UPROPERTY(Transient, BlueprintReadWrite)
	int TileSize;

	FInventoryInfo InventoryInfo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FIntPoint, FDSItemInfo> AllItems;

	// UI
	UPROPERTY(meta = (BindWidgetOptional),BlueprintReadWrite)
	TObjectPtr<UPanelWidget> GridBorder;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadWrite)
	TObjectPtr<UCanvasPanel> GridCanvasPanel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FDSLine> Lines;

	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UDSInventoryItem> InventoryItemClass;
	
};
