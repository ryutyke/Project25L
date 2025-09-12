#pragma once
// Default
#include "CoreMinimal.h"

// UE
#include "Engine/DataAsset.h"
#include "GameData/DSEnums.h"

// Game

// UHT
#include "DSDataTables.generated.h"

/**
 * 여러 UDataTable 인스턴스를 EDataTableType으로 매핑하여 관리하는 Data Asset.
 */

UCLASS()
class PROJECT25L_API UDSDataTables : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data Tables")
    TMap<EDataTableType, TObjectPtr<UDataTable>> DataTableMap;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data Tables")
    TMap<EDataTableType, TSoftObjectPtr<UDataTable>> SoftDataTableMap;
};
