#pragma once
// Default
#include "CoreMinimal.h"

// UE
#include "Engine/DataTable.h"

// Game
#include "GameData/DSEnums.h"

// UHT
#include "DSInputData.generated.h"

USTRUCT(BlueprintType)
struct FDSInputData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	FDSInputData() :
		InputThreshold(0.f)
	{ }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DSSettings | Input")
	float InputThreshold;

};