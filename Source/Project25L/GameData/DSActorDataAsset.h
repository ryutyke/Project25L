#pragma once

// Default
#include "CoreMinimal.h"

// UE
#include "Engine/DataAsset.h"

// Game
#include "GameData/DSPrimaryDataAsset.h"

// UHT
#include "DSActorDataAsset.generated.h"

UCLASS()
class PROJECT25L_API UDSActorDataAsset : public UDSPrimaryDataAsset
{
	GENERATED_BODY()
	

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DSSettings | Class")
	TSoftClassPtr<AActor> ActorClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DSSettings | Class")
	TSoftObjectPtr<UPhysicsAsset> PhysicsAsset;
};
