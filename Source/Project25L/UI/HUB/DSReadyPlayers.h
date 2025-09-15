#pragma once
// Default
#include "CoreMinimal.h"

// Game
#include "UI/Base/DSUserWidget.h"

// UHT
#include "DSReadyPlayers.generated.h"

class UTextBlock;

UCLASS()
class PROJECT25L_API UDSReadyPlayers : public UDSUserWidget
{
	GENERATED_BODY()
public:
	UDSReadyPlayers(const FObjectInitializer& ObjectInitializer);

	void UpdatePlayerCount(int CurrentPlayerCount, int MaxPlayerCount);

protected:


	UPROPERTY(EditAnywhere, Category = "DSSettings | Text")
	FString ReadyStatusText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> Text_Count;

};
