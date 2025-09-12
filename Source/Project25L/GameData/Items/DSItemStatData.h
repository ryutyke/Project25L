#pragma once

// Default
#include "CoreMinimal.h"

// UE
#include "Engine/DataTable.h"

// UHT
#include "DSItemStatData.generated.h"

class UTexture2D;

USTRUCT(BlueprintType)
struct FDSItemStatData: public FTableRowBase
{
	GENERATED_USTRUCT_BODY()


public:
	FDSItemStatData()
		: ItemDuration(0.0f)
		, ItemRange(0.f)
		, ItemDamage(0.f)
		, BoostHP(0.f)
		, AddMaxHP(0.f)
		, AddAttack(0.f)
		, AddDefense(0.f)
		, AddLuck(0.f)
		, AddMoveSpeed(0.f)
		, AddJumpHeight(0.f)
		, AddAttackSpeed(0.f)
		, AddHPRegen(0.f)
		, AddHPSteal(0.f)
		, AddAttackRange(0.f)
		, AddDamageBoost(0.f)
		, bCanFly(0)
		, AnimationOverride(nullptr)
	{}
	
	UPROPERTY(EditAnywhere, Category = Item)
	float ItemDuration;

	UPROPERTY(EditAnywhere, Category = Item)
	float ItemRange;

	UPROPERTY(EditAnywhere, Category = Item)
	float ItemDamage;
	
	UPROPERTY(EditAnywhere, Category = Item)
	float BoostHP;
	
	UPROPERTY(EditAnywhere, Category = Item)
	float AddMaxHP;
	
	UPROPERTY(EditAnywhere, Category = Item)
	float AddAttack;
	
	UPROPERTY(EditAnywhere, Category = Item)
	float AddDefense;

	UPROPERTY(EditAnywhere, Category = Item)
	float AddLuck;

	UPROPERTY(EditAnywhere, Category = Item)
	float AddMoveSpeed;

	UPROPERTY(EditAnywhere, Category = Item)
	float AddJumpHeight;

	UPROPERTY(EditAnywhere, Category = Item)
	float AddAttackSpeed;

	UPROPERTY(EditAnywhere, Category = Item)
	float AddHPRegen;

	UPROPERTY(EditAnywhere, Category = Item)
	float AddHPSteal;

	UPROPERTY(EditAnywhere, Category = Item)
	float AddAttackRange;

	UPROPERTY(EditAnywhere, Category = Item)
	float AddDamageBoost;

	UPROPERTY(EditAnywhere, Category = Item)
	bool bCanFly;

	UPROPERTY(EditAnywhere, Category = Item)
	TSoftObjectPtr<UAnimMontage> AnimationOverride;

};