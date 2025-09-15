#pragma once
// Default
#include "CoreMinimal.h"

// UE
#include "Engine/DataTable.h"
#include "GameData/DSEnums.h"

// Game
#include "Weapon/DSWeapon.h"

// UHT
#include "DSWeaponData.generated.h"

USTRUCT(BlueprintType)
struct FDSWeaponData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	FDSWeaponData() :
		Montages()
		, Weapon(nullptr)
		, SpreadCoef(0.f)
	{ }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DSSettings | Animation")
	TMap<EWeaponState, TSoftObjectPtr<UAnimMontage>> Montages;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DSSettings | Mesh")
	TSoftClassPtr<ADSWeapon> Weapon;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DSSettings | Gun")
	float SpreadCoef;
};