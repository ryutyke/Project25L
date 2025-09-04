#pragma once

// Default
#include "CoreMinimal.h"

// UE
#include "Engine/DataAsset.h"

// Game

// UHT
#include "DSPrimaryDataAsset.generated.h"


UCLASS()
class PROJECT25L_API UDSPrimaryDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	virtual uint32 GetKey() PURE_VIRTUAL(UDSPrimaryDataAsset::GetKey, return -1; );
};
