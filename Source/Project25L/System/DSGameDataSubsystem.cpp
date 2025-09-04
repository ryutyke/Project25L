// Default
#include "System/DSGameDataSubsystem.h"

// UE
#include "Subsystems/GameInstanceSubsystem.h"

// Game
#include "GameData/DSNonCharacterDataAsset.h"
#include "GameData/DSCharacterDataAsset.h"
#include "GameData/DSCharacterStat.h"
#include "GameData/DSDataTables.h"
#include "GameData/DSNonCharacterStat.h"
#include "GameData/Skill/DSComboActionData.h"

FStreamableManager UDSGameDataSubsystem::StreamableManager;

UDSGameDataSubsystem::UDSGameDataSubsystem()
	: DataTables(nullptr)
	, bIsAllAssetsLoaded(false)
	, PendingAssetLoadsCount(0)
{
	static const FSoftObjectPath DataTablesAssetPath(TEXT("/Game/GameData/DataAsset/DA_DataTables.DA_DataTables"));
	DataTablesAsset = DataTablesAssetPath;
}

void UDSGameDataSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	DSEVENT_DELEGATE_BIND(GameEvent.OnAllAssetsLoaded, this, &UDSGameDataSubsystem::HandleOnAllAssetsLoaded);

	LoadDataTables();
	LoadAllDataAssetsAsync();

}

void UDSGameDataSubsystem::Deinitialize()
{
	DataTables = nullptr;
	Super::Deinitialize();
}

void UDSGameDataSubsystem::LoadAllDataAssetsAsync()
{
	PendingAssetLoadsCount = 2;

	LoadDataAssetAsync<ECharacterType, UDSCharacterDataAsset>();
	LoadDataAssetAsync<int32, UDSNonCharacterDataAsset>();

}

void UDSGameDataSubsystem::LoadDataTables()
{
	if (DataTablesAsset.IsValid())
	{
		DataTables = DataTablesAsset.Get();
	}
	else if (DataTablesAsset.ToSoftObjectPath().IsValid())
	{
		DataTables = DataTablesAsset.LoadSynchronous();
		DS_LOG(DSDataLog, Log, TEXT("DataTables is loaded"));
	}

	if (nullptr == DataTables)
	{
		DS_LOG(DSDataLog, Warning, TEXT("DataTables asset is not loaded or assigned!"));
	}
}

void UDSGameDataSubsystem::HandleOnAllAssetsLoaded()
{
#if WITH_EDITOR
	if (GetDataAssetByType<ECharacterType, UDSCharacterDataAsset>(ECharacterType::None))
	{
		DS_LOG(DSDataLog, Log, TEXT("GetDataAssetByType Test"));
	}
#endif

}

UDSGameDataSubsystem* UDSGameDataSubsystem::Get(const UObject* WorldContextObject)
{
	if (!WorldContextObject)
	{
		return nullptr;
	}

	UWorld* World = WorldContextObject->GetWorld();
	check(World);

	UGameInstance* GameInstance = World->GetGameInstance();
	check(GameInstance);

	return GameInstance->GetSubsystem<UDSGameDataSubsystem>();
}

FTableRowBase* UDSGameDataSubsystem::GetDataRowByID(EDataTableType DataTableType, int32 DataID)
{
	UDataTable* DataTable = GetDataTable(DataTableType);

	if (IsValid(DataTable))
	{
		FString ID = FString::Printf(TEXT("%d"), DataID);

		FName DataName = FName(ID);

		FTableRowBase* Row = DataTable->FindRow<FTableRowBase>(DataName, ID);

		if (nullptr != Row)
		{
			return Row;
		}
	}

	return nullptr;
}

UDataTable* UDSGameDataSubsystem::GetDataTable(EDataTableType InTableType) const
{
	if (nullptr != DataTables)
	{
		const TObjectPtr<UDataTable>* FoundTable = DataTables->DataTableMap.Find(InTableType);

		if (nullptr != FoundTable)
		{
			return FoundTable->Get();
		}
	}

	DS_LOG(DSDataLog, Warning, TEXT("Requested DataTable for type %s not found!"), *UEnum::GetValueAsString(InTableType));
	return nullptr;
}