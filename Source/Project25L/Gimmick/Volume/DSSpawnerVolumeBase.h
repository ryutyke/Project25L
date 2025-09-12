#pragma once

// Defualt
#include "CoreMinimal.h"

// UE
#include "Engine/TriggerVolume.h"

// Game
#include "GameData/DSEnums.h"

// UHT
#include "DSSpawnerVolumeBase.generated.h"


UCLASS()
class PROJECT25L_API ADSSpawnerVolumeBase : public ATriggerVolume
{
	GENERATED_BODY()
	
public:

	ADSSpawnerVolumeBase();

protected:

	virtual void BeginPlay() override;

protected:
	FVector CalculateRandomPosition();

	void SpawnActors();
	// 고정된 범위내에서 아이템을 스폰
	void SpawnInFixedRange();
	// 고정된 위치에 아이템 스폰
	void SpawnAtFixedLocation();

protected:
	UPROPERTY(EditAnywhere, Category = "DSSettings | Spawner")
	int32 SpawnMax;

	UPROPERTY(EditAnywhere, Category = "DSSettings | Spawner")
	int32 SpawnMin;

	UPROPERTY(EditAnywhere, Category = "DSSettings | Spawner")
	ESpawnerType SpawnerType;

	UPROPERTY(EditAnywhere, Category = "DSSettings | Spawner")
	TArray<int32> SpawnIDs;

	UPROPERTY(EditInstanceOnly, Category = "DSSettings | Spawner")
	int32 SpawnGiftID;
};
