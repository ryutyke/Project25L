#pragma once

// Default
#include "CoreMinimal.h"

// UE
#include "Engine/DataTable.h"

// Game
#include "GameData/DSEnums.h"

// UHT
#include "DSItemData.generated.h"

class UTexture2D;
class UMaterialInterface;

USTRUCT(BlueprintType)
struct FDSItemData: public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	FDSItemData()
		: ItemName()
		, ItemHolder(0x00)
		, Probability(0.0f)
		, ActorClass(nullptr)
		, Description()
		, Tooltip()
	{}

	UPROPERTY(EditAnywhere, Category = "DSSettings | UI")
	FText ItemName;
	
	UPROPERTY(EditAnywhere, Category = "DSSettings | Character")
	uint8 ItemHolder;

	UPROPERTY(EditAnywhere, Category = "DSSettings | UI")
	TSoftObjectPtr<UTexture2D> ItemLogo;

	UPROPERTY(EditAnywhere, Category = "DSSettings | UI")
	TSoftObjectPtr<UMaterialInterface> ItemTileMaterial;

	UPROPERTY(EditAnywhere, Category = "DSSettings | UI")
	TSoftObjectPtr<UMaterialInterface> ItemTileMaterialRotated;

	UPROPERTY(EditAnywhere, Category = "DSSettings | Prop")
	float Probability;

	UPROPERTY(EditAnywhere, Category = "DSSettings | Actor")
	TSoftClassPtr<AActor> ActorClass;

	UPROPERTY(EditAnywhere, Category = "DSSettings | UI")
	FText Description;

	UPROPERTY(EditAnywhere, Category = "DSSettings | UI")
	FText Tooltip;

	UPROPERTY(EditAnywhere, Category = "DSSettings | UI")
	FIntPoint Dimensions;
	
};

USTRUCT(BlueprintType)
struct FDSItemInfo
{
	GENERATED_USTRUCT_BODY()

public:
	FDSItemInfo() {}

	FDSItemInfo(int32 ID) : ID(ID), isRotated(false) {}

	UPROPERTY()
	int32 ID;

	UPROPERTY()
	uint8 isRotated : 1;

	// 구조체의 변수들을 리플리케이션하기 위해 GetLifetimeReplicatedProps() 필요
	bool operator==(const FDSItemInfo& Other) const
	{
		return ID == Other.ID;
	}

	// NetSerialize 구현
	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
	{
		Ar << ID;
		bOutSuccess = true;
		return true;
	}
};

UCLASS()
class PROJECT25L_API UDSItemUI : public UObject
{
	GENERATED_BODY()
	
public:

	int32 ID;
	uint8 bIsSelected : 1;
};

USTRUCT(BlueprintType)
struct FInventoryInfo
{
	GENERATED_BODY()

	public:
	FInventoryInfo()
		: Columns(0), Rows(0), TileSize(0)
	{
	}

	UPROPERTY()
	int32 Columns;
	UPROPERTY()
	int32 Rows;
	UPROPERTY()
	int32 TileSize;

	FInventoryInfo(int32 InColumns, int32 InRows, int32 InTileSize)
		: Columns(InColumns), Rows(InRows), TileSize(InTileSize)
	{}
};

USTRUCT(BlueprintType)
struct FDSLine
{	
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D Start;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D End;

	FDSLine()
		: Start(FVector2D::ZeroVector), End(FVector2D::ZeroVector)
	{
	}

	FDSLine(float X1, float Y1, float X2, float Y2)
		: Start(FVector2D(X1, Y1)), End(FVector2D(X2, Y2))
	{
	}

};
