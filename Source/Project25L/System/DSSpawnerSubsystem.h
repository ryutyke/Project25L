#pragma once

// Default
#include "CoreMinimal.h"

// UE
#include "Subsystems/WorldSubsystem.h"

// Game
#include "GameData/DSEnums.h"

// UHT
#include "DSSpawnerSubsystem.generated.h"


USTRUCT()
struct FSpawnItemInfo
{
	GENERATED_BODY()

public:
	FSpawnItemInfo() {}
	FSpawnItemInfo(int32 ItemID, float Prob)
		: ItemID(ItemID)
		, SpawnProbability(Prob) {}

	int32 ItemID;
	float SpawnProbability;
};

UCLASS()
class PROJECT25L_API UDSSpawnerSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
	UDSSpawnerSubsystem();

	static UDSSpawnerSubsystem* Get(UObject* Object);

	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

	/*
	* 생성할 아이템 개수 Min - Max 값을 전달하면 랜덤으로 개수를 생성한다.
	* 정규 분포식을 사용해서 나온 아이템 개수와 아이템 아이디를 리턴하는 함수
	*/
	TMap<int32, int32> SelectChestItems(TArray<int32>& ItemIDs, int32 MaxRange, int32 MinRange, FVector& Location);
	
	AActor* CreateActor(ESpawnerType SpawnType, int32 SpawnID, FVector& Location);

protected:

	void InitializeData();

	// 정규분포의 값 중 랜덤으로 아이템을 뽑을 함수
	int32 GetRandomItemID(TArray<int32>& ItemIDs, float TotalProbability);

	// 아이템에 대한 정규분포의 총 값을 리턴하는 함수
	float GetTotalItemProbability(TArray<int32>& ItemIDs);
protected:

	UPROPERTY(Transient)
	TArray<FSpawnItemInfo> ItemInfo;

};
