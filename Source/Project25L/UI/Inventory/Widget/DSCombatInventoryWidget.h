#pragma once

// Default
#include "CoreMinimal.h"

// Game
#include "UI/Base/DSUserWidget.h"
#include "GameData/DSEnums.h"
#include "GameData/Items/DSItemData.h"

// UHT
#include "DSCombatInventoryWidget.generated.h"


class UDSInventoryGridWidget;
class UDSItemTooltip;
class UDSPlayerSendPanel;

UCLASS()
class PROJECT25L_API UDSCombatInventoryWidget : public UDSUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	
	void SetItemToolTip(const FDSItemInfo& ItemInfo, FIntPoint TopLeftIndex, EInventoryCategory Category, int32 TileSize, bool bIsVisible);

	// 플레이어 ID sendwidget에 부여하기
	void SetPlayerIdsForSendWidgets();

	void InitInventoryWidget();
	virtual void BindEvents() override;
	virtual void InitializeWidget() override;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UDSInventoryGridWidget> PersonalItemInventoryWidget;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UDSInventoryGridWidget> PersonalVehicleInventoryWidget;

	UPROPERTY(Transient, meta = (BindWidgetOptional))
	TObjectPtr<UDSItemTooltip> ItemTooltips;


	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UDSPlayerSendPanel> SendItemWidget0;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UDSPlayerSendPanel> SendItemWidget1;

};
