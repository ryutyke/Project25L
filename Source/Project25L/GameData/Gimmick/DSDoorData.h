#pragma once

// Default
#include "CoreMinimal.h"

// UE
#include "Engine/DataAsset.h"

// Game
#include "GameData/DSEnums.h"

// UHT
#include "DSDoorData.generated.h"

class UMaterialInterface;
class ULevelSequence;
class AActor;

UCLASS()
class PROJECT25L_API UDSDoorData : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, Category = "DSSettings | Change")
	EDungeonType DungeonType;
	
	UPROPERTY(EditAnywhere, Category = "DSSettings | Change")
	TSoftClassPtr<AActor> Bead;
	
	UPROPERTY(EditAnywhere, Category = "DSSettings | Change")
	TObjectPtr<UMaterialInterface> ChangedBeadMateral;

	//Chaos Destruction Cache Collection
	UPROPERTY(EditAnywhere, Category = "DSSettings | Change")
	TSoftObjectPtr<ULevelSequence> DestroyDoorSequence;
	
	UPROPERTY(EditAnywhere, Category = "DSSettings | Change")
	FString Destination;
};
