// Default
#include "DSTimerPanel.h"

// UE
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"

UDSTimerPanel::UDSTimerPanel(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
}

void UDSTimerPanel::StartCountdown()
{
	RemainingTime = CountdownDuration;

    GetWorld()->GetTimerManager().SetTimer(
        CountdownTimerHandle, this, 
        &UDSTimerPanel::UpdateCountdown,
        UpdateInterval, true
    );
}

void UDSTimerPanel::UpdateCountdown()
{
    RemainingTime -= UpdateInterval;

    const float ClampedTime = FMath::Max(0.0f, RemainingTime);
    const int32 Minutes = FMath::FloorToInt(ClampedTime / 60.0f);
    const int32 Seconds = FMath::FloorToInt(FMath::Fmod(ClampedTime, 60.0f));

    const FString TimeString = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
    RemainingTimeText->SetText(FText::FromString(TimeString));

    const float Ratio = FMath::Clamp(ClampedTime / CountdownDuration, 0.0f, 1.0f);
    CountdownProgressBar->SetPercent(Ratio);

   for (const TPair<float, FLinearColor>& Pair : ProgressBarColorMap)
   {
	   if (RemainingTime >= Pair.Key)
	   {
        // 첫 번째 조건 만족한 색상만 사용
		   CountdownProgressBar->SetFillColorAndOpacity(Pair.Value);
		   break; 
	   }
   }

    if (RemainingTime <= 0.0f)
    {
        FinishCountdown();
    }
}

void UDSTimerPanel::FinishCountdown()
{
    GetWorld()->GetTimerManager().ClearTimer(CountdownTimerHandle);
}
