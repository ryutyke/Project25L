#pragma once
// Default
#include "CoreMinimal.h"

// UE
#include "Blueprint/UserWidget.h"

// Game
#include "Interface/UIEventListener.h"

// UHT
#include "DSUserWidget.generated.h"

UCLASS()
class PROJECT25L_API UDSUserWidget : public UUserWidget, public IUIEventListener
{
	GENERATED_BODY()

public:
	UDSUserWidget(const FObjectInitializer& ObjectInitializer);
	virtual void BindEvents() { }
	virtual void InitializeWidget() {}
};
