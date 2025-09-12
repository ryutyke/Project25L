#pragma once
// Default
#include "CoreMinimal.h"

// Game
#include "UI/Base/DSUserWidget.h"

// UHT
#include "DSReadyBoardPanel.generated.h"

class UDSReadyBoardEntry;
class UButton;
class UHorizontalBox;

UCLASS()
class PROJECT25L_API UDSReadyBoardPanel : public UDSUserWidget
{
	GENERATED_BODY()
	
public:
	UDSReadyBoardPanel(const FObjectInitializer& ObjectInitializer);
	virtual void NativeConstruct() override;
	virtual void InitializeWidget() override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DSSettings | ReadyEntry", Meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UDSReadyBoardEntry> ReadyBoardEntryClass;

	UPROPERTY()
	TArray<UDSReadyBoardEntry*> ReadyBoardEntryArray;

	UPROPERTY(EditAnywhere, meta = (BindWidgetOptions))
	TObjectPtr<UHorizontalBox> ReadyBoardEntryBox;

	UPROPERTY(EditAnywhere, meta = (BindWidgetOptions))
	TObjectPtr<UButton> SendToHubButton;
};
