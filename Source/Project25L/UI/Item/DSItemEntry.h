#pragma once

// Default
#include "CoreMinimal.h"

// UE
#include "Blueprint/IUserObjectListEntry.h"

// Game
#include "UI/Base/DSFocusedWidget.h"

// UHT
#include "DSItemEntry.generated.h"

class UImage;
class UTextBlock;

UCLASS()
class PROJECT25L_API UDSItemEntry : public UDSFocusedWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
	
public:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

	void SetFocus(bool bIsFocus);

protected:
	
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> IMG_Background;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> IMG_ItemIcon;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> Text_ItemName;

};
