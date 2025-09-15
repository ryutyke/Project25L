#pragma once
// Default
#include "CoreMinimal.h"

// UE
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "GameData/DSEnums.h"

// Game
#include "DSLogChannels.h"
#include "System/DSEventSystems.h"

// UHT
#include "DSGameDataSubsystem.generated.h"


class UDSNonCharacterDataAsset;
class UDSCharacterDataAsset;
class UDSComboActionData;
class UDSDataTables;
struct FDSCharacterStat;
struct FDSNonCharacterStat;
/**
 * @brief 게임 인스턴스 수준에서 다양한 데이터 에셋 및 데이터 테이블을 관리하는 서브시스템.
 */
UCLASS()
class PROJECT25L_API UDSGameDataSubsystem final : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	UDSGameDataSubsystem();

	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
 	static UDSGameDataSubsystem* Get(const UObject* WorldContextObject);
public:
	
	/**
	 * @brief DataAsset을 관리하는 TMap에서 열거형 값에 해당하는 데이터 에셋을 반환한다.
	 *
	 * TMap은 템플릿 기반 타입별 싱글톤 인스턴스이다.
	 *
	 * @tparam KeyType		TMap의 Key에 해당하는 열거형 타입 (예: 캐릭터 데이터의 경우 ECharacterType과 같이 캐릭터 종류를 나타내는 열거형)
	 * @tparam AssetType	TMap의 Value에 해당하는 DataAsset 클래스 타입 (예: 캐릭터 데이터를 위한 UDSCharacterDataAsset)
	 *	
	 * @param InKey 찾고자 하는 데이터 에셋에 해당하는 Enum 값 (예: ECharacterType::Girl)
	 * 
	 * @return AssetType*   열거형 값에 해당하는 데이터 에셋
	 */
	template<typename KeyType, typename AssetType>
	const AssetType* GetDataAssetByType(KeyType InKey);

	/**
	 * @brief Enum 값에 해당하는 이름의 행 데이터(FTableRowBase)를 반환하는 템플릿 함수.
	 *
	 * 주어진 DataType 값을 문자열 변환 후 "::"로 분리해 행 이름을 추출합니다.
	 * 주어진 EDataTableType에 해당하는 데이터 테이블에 해당 행(TRowType)이 있으면 포인터를, 없으면 nullptr을 반환합니다
	 *
	 * @tparam TRowType		반환할 행 데이터 구조체의 타입 (예: 캐릭터 스탯 정보를 담은 FDSCharacterStat)
	 * @tparam TEnumType    데이터 테이블 행을 식별하는 Enum 타입 (예: 캐릭터 타입을 구분하는 ECharacterType)
	 *
	 * @param DataTableType 검색할 데이터 테이블의 종류 (EDataTableType)
	 * @param DataType		찾고자 하는 행 데이터에 해당하는 Enum 값 (예: ECharacterType::Girl)
	 *
	 * @return TRowType*   검색된 행 데이터의 포인터, 찾을 수 없는 경우 nullptr 반환
	 */
	template<typename TRowType, typename TEnumType>
	TRowType* GetDataRowByEnum(EDataTableType DataTableType, TEnumType DataType);

	/**
	 * @brief ID에 해당하는 이름의 행 데이터(FTableRowBase)를 반환하는 함수.
	 *
	 * @param DataTableType 검색할 데이터 테이블의 종류 (EDataTableType)
	 * @param DataID		찾고자 하는 행 데이터에 해당하는 ID 값 (예: 202)
	 *
	 * @return FTableRowBase*   검색된 행 데이터의 포인터, 찾을 수 없는 경우 nullptr 반환
	 */
	FTableRowBase* GetDataRowByID(EDataTableType DataTableType, int32 DataID);
	
	/**
	 * @brief DataTable들을 관리하는 DataTables 데이터 에셋을 반환하는 함수.
	 */
	UFUNCTION(BlueprintCallable, Category = "Data Table")
	const UDSDataTables* GetDataTables() const { return DataTables; }

	/**
	 * @brief 지정한 EDataTableType에 해당하는 UDataTable을 반환하는 함수.
	 */
	UFUNCTION(BlueprintCallable, Category = "Data Table")
	UDataTable* GetDataTable(EDataTableType TableType) const;

	UPROPERTY(EditDefaultsOnly, Category = "Data Table")
	TSoftObjectPtr<UDSDataTables> DataTablesAsset;

	/*
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Data Map")
	TMap<ECharacterType, UDSCharacterDataAsset*> CharacterDataMap;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Data Map")
	TMap<int32, UDSNonCharacterDataAsset*> EnemyDataMap;
	*/

	static FStreamableManager StreamableManager;

protected:
	UPROPERTY(Transient)
	UDSDataTables* DataTables;

	UPROPERTY(Transient)
	bool bIsAllAssetsLoaded;

	/** 비동기 로드해야 하는 Data Asset 종류(번들) 개수. */
	UPROPERTY(Transient)
	int32 PendingAssetLoadsCount;

	/**
	 * @brief 지정한 AssetType에 해당하는 DataAsset들을 비동기 로드하여, 전역 TMap에 등록하는 템플릿 함수.
	 *
	 * UAssetManager를 통해 TAssetType에 해당하는 PrimaryAssetId 목록을 받아,
	 * 각 Asset을 비동기 로드 후 GetKey()로 고유 키(TKeyType)를 추출하여 OutDataAssetMap에 등록합니다.
	 * 이때 중복 키는 무시하고, 로드 실패 시 로그를 남깁니다.
	 * 모든 Asset이 로드되면 bIsAllAssetsLoaded를 true로 설정 후 OnAllAssetsLoaded 델리게이트를 broadcast합니다.
	 *
	 * @tparam KeyType		TMap 등록 시 사용될 키의 타입 (예: 캐릭터 데이터의 경우 ECharacterType과 같이 캐릭터 종류를 나타내는 열거형)
	 * @tparam AssetType	로드할 DataAsset 클래스의 타입 (예: 캐릭터 데이터를 위한 UDSCharacterDataAsset)
	 */
	template<typename KeyType, typename AssetType>
	void LoadDataAssetAsync();

	void LoadAllDataAssetsAsync();
	void LoadDataTables();

	/**
	 * @brief DataAsset을 관리하는 TMap 싱글톤 인스턴스를 반환한다.
	 * 
	 * TMap은 템플릿 기반 타입별 싱글톤 인스턴스이다.
	 *
	 * @tparam KeyType		TMap의 Key에 해당하는 열거형 타입 (예: 캐릭터 데이터의 경우 ECharacterType과 같이 캐릭터 종류를 나타내는 열거형)
	 * @tparam AssetType	TMap의 Value에 해당하는 DataAsset 클래스 타입 (예: 캐릭터 데이터를 위한 UDSCharacterDataAsset)
	 *
	 * @return TMap<KeyType, AssetType*>&   로드된 DataAsset들을 저장하는 TMap 싱글톤 인스턴스.
	 */
	template<typename KeyType, typename AssetType>
	TMap<KeyType, AssetType*>& GetDataMap();

	void HandleOnAllAssetsLoaded();

};



template<typename KeyType, typename AssetType>
void UDSGameDataSubsystem::LoadDataAssetAsync()
{
	TMap<KeyType, AssetType*>& OutDataAssetMap = GetDataMap<KeyType, AssetType>();

	UAssetManager& AssetManager = UAssetManager::Get();
	TArray<FPrimaryAssetId> AssetIds;
	const FPrimaryAssetType& ThisPrimaryAssetType = AssetType::StaticClass()->GetDefaultObject()->GetPrimaryAssetId().PrimaryAssetType;

	AssetManager.GetPrimaryAssetIdList(ThisPrimaryAssetType, AssetIds);
	if (AssetIds.Num() == 0)
	{
		DS_LOG(DSDataLog, Warning, TEXT("No assets of type %s found!"), *ThisPrimaryAssetType.ToString());
		return;
	}

	TWeakObjectPtr<UDSGameDataSubsystem> WeakThis = this;
	AssetManager.LoadPrimaryAssets(AssetIds, TArray<FName>(), FStreamableDelegate::CreateLambda([WeakThis, AssetIds, ThisPrimaryAssetType, &OutDataAssetMap]()
		{
			if (!WeakThis.IsValid())
			{
				DS_LOG(DSDataLog, Warning, TEXT("Owner object is no longer valid. Aborting asset loading."));
				return;
			}

			UAssetManager& AssetManager = UAssetManager::Get();
			for (const FPrimaryAssetId& AssetId : AssetIds)
			{
				AssetType* AssetPtr = Cast<AssetType>(AssetManager.GetPrimaryAssetObject(AssetId));
				
				if (nullptr != AssetPtr)
				{
					KeyType Key = static_cast<KeyType>(AssetPtr->GetKey());

					if (OutDataAssetMap.Contains(Key))
					{
						DS_LOG(DSDataLog, Warning, TEXT("Duplicate asset detected. Asset '%s' is skipped because a data asset for this key is already loaded."), *AssetPtr->GetName());
					}
					else
					{
						OutDataAssetMap.Add(Key, AssetPtr);
						DS_LOG(DSDataLog, Log, TEXT("Loaded asset: %s"), *AssetPtr->GetName());
					}
				}
				else
				{
					DS_LOG(DSDataLog, Error, TEXT("Failed to load asset: %s"), *AssetId.ToString());
				}
			}
			DS_LOG(DSDataLog, Log, TEXT("All assets of type %s loaded successfully!"), *ThisPrimaryAssetType.ToString());

			if (--WeakThis->PendingAssetLoadsCount == 0)
			{
				DS_LOG(DSDataLog, Log, TEXT("All assets loaded successfully!"));
				WeakThis->bIsAllAssetsLoaded = true;
				DSEVENT_DELEGATE_INVOKE(GameEvent.OnAllAssetsLoaded);
			}
		}));
}

template<typename TRowType, typename TEnumType>
TRowType* UDSGameDataSubsystem::GetDataRowByEnum(EDataTableType DataTableType, TEnumType DataType)
{
	UDataTable* DataTable = GetDataTable(DataTableType);

	if (IsValid(DataTable))
	{
		FString DataName = UEnum::GetValueAsString(DataType);
		FString Left, Right;
		DataName.Split(TEXT("::"), &Left, &Right);

		FName DataRightName = FName(Right);

		TRowType* Row = DataTable->FindRow<TRowType>(DataRightName, DataRightName.ToString());

		if (nullptr != Row)
		{
			return Row;
		}
	}

	return nullptr;
}

template<typename KeyType, typename AssetType>
TMap<KeyType, AssetType*>& UDSGameDataSubsystem::GetDataMap()
{
	static TMap<KeyType, AssetType*> DataMap;
	return DataMap;
}

template<typename KeyType, typename AssetType>
const AssetType* UDSGameDataSubsystem::GetDataAssetByType(KeyType InKey)
{
	const TMap<KeyType, AssetType*>& DataMap = GetDataMap<KeyType, AssetType>();
	const AssetType* const* FoundPtr = DataMap.Find(InKey);
	
	if (nullptr != FoundPtr && IsValid(*FoundPtr))
	{
		return *FoundPtr;
	}
	else
	{
		DS_LOG(DSDataLog, Log, TEXT("Failed to find the Data Asset by type"));
	}
	return nullptr;
}