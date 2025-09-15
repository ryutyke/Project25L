#pragma once

// Default
#include "CoreMinimal.h"

// UE
#include "UObject/Interface.h"

// Game
#include "GameData/DSEnums.h"

// UHT
#include "HealableInterface.generated.h"

UINTERFACE(MinimalAPI)
class UHealableInterface : public UInterface
{
	GENERATED_BODY()
};


class PROJECT25L_API IHealableInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Heal")
	void RecoverHealth(float HealAmount, EHealType HealType, UObject* Instigator);
};
