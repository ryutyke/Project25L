#pragma once

// Default
#include "CoreMinimal.h"

// Game
#include "GameData/DSCharacterStat.h"

// UHT
#include "DSNonCharacterStat.generated.h"

USTRUCT(BlueprintType)
struct FDSNonCharacterStat : public FDSCharacterStat
{
	GENERATED_USTRUCT_BODY()

public:
	FDSNonCharacterStat() :
		Type(0)
		{ }

	int32 Type;
};