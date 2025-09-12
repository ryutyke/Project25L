#pragma once

// Default
#include "CoreMinimal.h"

// UE
#include "Engine/DamageEvents.h"

// Game
#include "GameData/DSEnums.h"

// UHT
#include "DSDamageEvents.generated.h"

class UDamageType;

USTRUCT(BlueprintType)
struct FDSDamageEvent : public FDamageEvent
{
	GENERATED_BODY()

public:
	EDamageType DamageType;
	
	EDSElementType ElementType;

	/** Default constructor (no initialization). */
	FDSDamageEvent() { }

	FDSDamageEvent(FDSDamageEvent const& InDamageEvent)
		: Super(InDamageEvent)
	{ }
	
	virtual ~FDSDamageEvent() { }

	explicit FDSDamageEvent(TSubclassOf<UDamageType> InDamageTypeClass)
		: Super(InDamageTypeClass)
	{ }
};