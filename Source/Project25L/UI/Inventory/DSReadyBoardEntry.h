#pragma once
// Default
#include "CoreMinimal.h"
// Game
#include "UI/Base/DSUserWidget.h"
// UHT
#include "DSReadyBoardEntry.generated.h"

class UTextBlock;
class UImage;

UCLASS()
class PROJECT25L_API UDSReadyBoardEntry : public UDSUserWidget
{
	GENERATED_BODY()

public:
	UDSReadyBoardEntry(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ReadyText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> ReadyImage;
};
