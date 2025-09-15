#pragma once

// Default
#include "CoreMinimal.h"

// UE
#include "Engine/AssetManager.h"

// UHT
#include "DSAssetManager.generated.h"


UCLASS()
class PROJECT25L_API UDSAssetManager : public UAssetManager
{
	GENERATED_BODY()
public:
	static UDSAssetManager& Get();

protected:
	virtual void StartInitialLoading() override;
};
