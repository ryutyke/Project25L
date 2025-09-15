#pragma once

// Default
#include "CoreMinimal.h"

// Game
#include "UI/Base/DSUserWidget.h"
#include "GameData/DSEnums.h"

// UHT
#include "DSMainWidget.generated.h"

class UVerticalBox;
class UDSPlayerInfo;
class UDSHPBar;
class UDSItemPickupSlot;
class UOverlay;
class UTextBlock;

UCLASS()
class PROJECT25L_API UDSMainWidget : public UDSUserWidget
{
	GENERATED_BODY()

public:
	UDSMainWidget(const FObjectInitializer& ObjectInitializer);
	virtual void NativeConstruct() override;

public:
	UDSHPBar* GetHPBar(int32 PlayerIndex);
	UDSPlayerInfo* GetPlayerInfo(int32 PlayerIndex);
	UDSItemPickupSlot* GetItemPickup() { return ItemPickupSlot; }

protected:

	void InvalidateWidget();
	void SetupItemPickupSlot(int32 ItemID, bool bRemoved);
	void SetupAlertComment(EAlertStatus AlertStatus);
	void HideAlertOverlay();

protected:

	/*0번이 개인, 나머진 각자*/
	UPROPERTY(Transient)
	TArray<TObjectPtr<UDSPlayerInfo>> PlayerInfos;
protected:
	
	UPROPERTY(EditAnywhere, Category = "DSSettings | Text")
	TMap<EAlertStatus,FText> AlertComment;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UVerticalBox> VBox_Info;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UDSPlayerInfo> PlayerInfo;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UDSItemPickupSlot> ItemPickupSlot;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UOverlay> Overlay_AlertType;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> Text_AlertComment;

};
