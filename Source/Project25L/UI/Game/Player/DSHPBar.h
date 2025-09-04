#pragma once
// Default
#include "CoreMinimal.h"

// Game
#include "UI/Base/DSUserWidget.h"

// UHT
#include "DSHPBar.generated.h"

class UProgressBar;

UCLASS()
class PROJECT25L_API UDSHPBar : public UDSUserWidget
{
	GENERATED_BODY()
	
public:
	UDSHPBar(const FObjectInitializer& ObjectInitializer);

	// 전달된 HP는 0~100 퍼센트 기준 (ex. 75.0f = 75%)
	void SetHP(float CurrentHP, float MaxHP);
protected:

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UProgressBar> ProgressBar_HP;
};
