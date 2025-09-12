#pragma once

// Default
#include "CoreMinimal.h"

// Game
#include "UI/Base/DSUserWidget.h"

//UHT
#include "DSTimerPanel.generated.h"

class UProgressBar;
class UTextBlock;

UCLASS()
class PROJECT25L_API UDSTimerPanel : public UDSUserWidget
{
	GENERATED_BODY()
	
public:
	UDSTimerPanel(const FObjectInitializer& ObjectInitializer);

	void StartCountdown();
	void UpdateCountdown();
	void FinishCountdown();

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> CountdownProgressBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> RemainingTimeText;

	UPROPERTY( EditAnywhere, Category = "DSSettings|TimerUpdateInterval")
	float UpdateInterval = 1.0f;
	// DataSubsystem에서 가지고 오기
	UPROPERTY( EditAnywhere, Category = "DSSettings|TimerDurationSeconds")
	float CountdownDuration = 5.0f;
	UPROPERTY( EditAnywhere, Category = "DSSettings|ProgressBarColors")
	TMap<float, FLinearColor> ProgressBarColorMap;

private:
	FTimerHandle CountdownTimerHandle;
	float RemainingTime = 0.0f;
};
