#pragma once
// Default
#include "CoreMinimal.h"
// Game
#include "DSUserWidget.h"
// UHT
#include "DSFocusedWidget.generated.h"

class UTexture2D;
class UImage;

UCLASS()
class PROJECT25L_API UDSFocusedWidget : public UDSUserWidget
{
	GENERATED_BODY()
	
public:
	UDSFocusedWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;
	UTexture2D* LoadTexture(TSoftObjectPtr<UTexture2D> &Data);

	void SetFocus(UImage* Image, bool bIsFocus);
protected:
	UPROPERTY(Transient)
	TObjectPtr<UTexture2D> FocusedIcon;

	UPROPERTY(Transient)
	TObjectPtr<UTexture2D> DefaultIcon;

	UPROPERTY(EditDefaultsOnly, Category = "DSSettings | Texture")
	TSoftObjectPtr<UTexture2D> FocusedTexture;

	UPROPERTY(EditDefaultsOnly, Category = "DSSettings | Texture")
	TSoftObjectPtr<UTexture2D> DefaultTexture;
};
