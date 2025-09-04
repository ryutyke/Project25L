// Default
#include "System/DSSpawnerSubsystem.h"

// UE

// Game
#include "Character/Enemies/DSNonCharacter.h"
#include "System/DSGameDataSubsystem.h"
#include "GameData/DSNonCharacterDataAsset.h"
#include "GameData/Items/DSItemData.h"

#include "DSLogChannels.h"

UDSSpawnerSubsystem::UDSSpawnerSubsystem()
	: Super()
	, ItemInfo()
{
}

UDSSpawnerSubsystem* UDSSpawnerSubsystem::Get(UObject* Object)
{
	UWorld* World = Object->GetWorld();

	check(World);

	return World->GetSubsystem<UDSSpawnerSubsystem>();
}

void UDSSpawnerSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

	InitializeData();
}

void UDSSpawnerSubsystem::InitializeData()
{
	UDSGameDataSubsystem* DataSubsystem = UDSGameDataSubsystem::Get(this);

	check(DataSubsystem);

	UDataTable* ItemDataTable = DataSubsystem->GetDataTable(EDataTableType::ItemData);

	if (false == IsValid(ItemDataTable))
	{
		return;
	}

	TMap<FName, uint8*> RowMap = ItemDataTable->GetRowMap();

	// SpawnerSubsystem에서 필요로 하는 필수 종류만 구조체로 가지고 있는다.
	for (const auto& Row : RowMap)
	{
		FDSItemData* ItemData = reinterpret_cast<FDSItemData*>(Row.Value);
		if (nullptr != ItemData)
		{
			int ItemID = FCString::Atoi(*(Row.Key.ToString()));

			ItemInfo.Add({ ItemID, ItemData->Probability });
		}
	}
}


TMap<int32, int32> UDSSpawnerSubsystem::SelectChestItems(TArray<int32>& ItemIDs, int32 MaxRange, int32 MinRange, FVector& Location)
{
	UWorld* World = GetWorld();

	check(World);

	UDSGameDataSubsystem* DataSubsystem = UDSGameDataSubsystem::Get(this);

	check(DataSubsystem);

	int32 RandomCount = FMath::RandRange(MinRange, MaxRange);

	TMap<int32, int32> Result;
	int32 ItemID = 0;

	float TotalProbability = GetTotalItemProbability(ItemIDs);

	for (int32 RandomIdx = 0; RandomIdx < RandomCount; RandomIdx++)
	{
		ItemID = GetRandomItemID(ItemIDs, TotalProbability);

		//아이템이 0번인 경우는 없음.

		if (0 == ItemID)
		{
			continue;
		}
		
		if (false == Result.Contains(ItemID))
		{
			Result.Add({ ItemID, 1 });
		}
		else
		{
			Result[ItemID]++;
		}

		DS_LOG(DSItemLog, Log, TEXT("Selected Item %d, Spawn!"), ItemID);
	}

	return Result;
}

AActor* UDSSpawnerSubsystem::CreateActor(ESpawnerType SpawnType, int32 SpawnID, FVector& Location)
{
	/*
	몬스터랑 아이템 스포너 둘을 비슷하도록 만든다.
	현재 아이템 스포너 int32 아이디를 가지는 것처럼 몬스터 스포너 또한, int32를 사용해서 아이디를 가질 수 있도록 만든다.
	*/

	//지정된 몬스터 위치에 몬스터를 만든다.
	UDSGameDataSubsystem* DataSubsystem = UDSGameDataSubsystem::Get(this);

	check(DataSubsystem);
	
	TSubclassOf<AActor> ActorClass = nullptr;

	if (SpawnType == ESpawnerType::RangeMonster)
	{
		const UDSNonCharacterDataAsset* DataAsset = DataSubsystem->GetDataAssetByType<int32, UDSNonCharacterDataAsset>(SpawnID);
		
		if (nullptr != DataAsset)
		{
			ActorClass = DataAsset->ActorClass.LoadSynchronous();
		}
	}
	else
	{
		const FDSItemData* ItemData = static_cast<FDSItemData*>(DataSubsystem->GetDataRowByID(EDataTableType::ItemData, SpawnID));

		if (nullptr != ItemData)
		{
			ActorClass = ItemData->ActorClass.LoadSynchronous();
		}
	}

	AActor* SpawnObj = nullptr;

	if (IsValid(ActorClass))
	{
		UWorld* World = GetWorld();

		check(World);

		FActorSpawnParameters Params;

		SpawnObj = World->SpawnActor<AActor>(ActorClass, Location, FRotator::ZeroRotator, Params);
	}

	return SpawnObj;
}

int32 UDSSpawnerSubsystem::GetRandomItemID(TArray<int32>& ItemIDs, float TotalProbability)
{
	// 랜덤으로 뽑아서,
	float RandomValue = FMath::FRandRange(0.0f, TotalProbability);

	// 누적 값이 RandomValue보다 높아질 경우, ID를 반환한다.
	float AccumulatedProbability = 0.0f;

	for (const FSpawnItemInfo& Item : ItemInfo)
	{

		if (false == ItemIDs.Contains(Item.ItemID))
		{
			continue;
		}

		AccumulatedProbability += Item.SpawnProbability;
		if (RandomValue <= AccumulatedProbability)
		{
			return Item.ItemID;
		}
	}

	return 0;
}

float UDSSpawnerSubsystem::GetTotalItemProbability(TArray<int32>& ItemIDs)
{
	float TotalProbability = 0.0f;

	// 정규분포를 위해서 ID가 있을 경우에 더한다.
	for (int ItemIdx = 0; ItemIdx < ItemIDs.Num(); ItemIdx++)
	{
		for (int ArrayIdx = 0; ArrayIdx < ItemInfo.Num(); ArrayIdx++)
		{
			if (ItemIDs[ItemIdx] == ItemInfo[ArrayIdx].ItemID)
			{
				TotalProbability += ItemInfo[ArrayIdx].SpawnProbability;
				break;
			}
		}
	}

	return TotalProbability;
}
