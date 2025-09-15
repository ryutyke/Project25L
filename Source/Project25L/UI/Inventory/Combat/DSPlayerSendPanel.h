#pragma once

// Default
#include "CoreMinimal.h"

// UE
#include "Blueprint/UserWidget.h"
#include "UI/Base/DSFocusedWidget.h"

// UHT
#include "DSPlayerSendPanel.generated.h"


class UTexture2D;
class UImage;
class UTextBlock;

UCLASS()
class PROJECT25L_API UDSPlayerSendPanel : public UDSFocusedWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	void SetPlayerId(int32 ID);

protected:

	UPROPERTY(Transient)
	int32 PlayerId;

	UPROPERTY(Transient, meta = (BindWidgetOptional))
	TObjectPtr<UImage> SendImage;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> Text_PlayerName;
};
