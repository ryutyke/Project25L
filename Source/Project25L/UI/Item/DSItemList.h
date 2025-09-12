#pragma once

// Default
#include "CoreMinimal.h"

// Game
#include "UI/Base/DSUserWidget.h"
#include "GameData/Items/DSItemData.h"

// UHT
#include "DSItemList.generated.h"


class UDSItemListView;
class UDSItemTooltip;
class UCanvasPanel;

UCLASS()
class PROJECT25L_API UDSItemList : public UDSUserWidget
{
	GENERATED_BODY()
	
public:

	UDSItemList(const FObjectInitializer& ObjectInitializer);
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	void AddItems(TArray<FDSItemInfo>& ItemInfos);
	void RemoveItem(int32 IndexToRemove);

protected:

	void InitItemList();
	void FocusOnItemEntry(int PreSelectedIdx);
	void OnListEntryHovered(int HoveredIdx);
	void OnListEntryUnhovered(int UnHoveredIdx);

	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

protected:
	UPROPERTY(Transient)
	int32 SelectedIdx;
	
	UPROPERTY(Transient)
	TArray<TObjectPtr<UDSItemTooltip>> ItemTooltips;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidgetOptional))
	TObjectPtr<UDSItemListView> ListView_Items;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidgetOptional))
	TObjectPtr<UCanvasPanel> Canvas_ItemTooltip;
	
};
