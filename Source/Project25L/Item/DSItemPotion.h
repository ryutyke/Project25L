#pragma once

// Default
#include "CoreMinimal.h"

// UE
#include "UObject/NoExportTypes.h"

// Game
#include "Item/DSItem.h"

// UHT
#include "DSItemPotion.generated.h"

UCLASS()
class PROJECT25L_API UDSItemPotion : public UObject, public IDSItem
{
	GENERATED_BODY()
	
public:
	UDSItemPotion();

	virtual void Initialize(FTableRowBase* DataTableRow) override;
	virtual void UseItem(UDSStatComponent* Stat) override;

protected:
	UPROPERTY(Transient)
	float Duration;

	UPROPERTY(Transient)
	float BoostHP;

	UPROPERTY(Transient)
	float AddMaxHP;

	UPROPERTY(Transient)
	float AddAttack;

	UPROPERTY(Transient)
	float AddDefense;

	UPROPERTY(Transient)
	float AddLuck;

	UPROPERTY(Transient)
	float AddMoveSpeed;

	UPROPERTY(Transient)
	float AddJumpHeight;

	UPROPERTY(Transient)
	float AddAttackSpeed;

	UPROPERTY(Transient)
	float AddHPRegen;

	UPROPERTY(Transient)
	float AddHPSteal;

	UPROPERTY(Transient)
	float AddAttackRange;

	UPROPERTY(Transient)
	float AddDamageBoost;

	UPROPERTY(Transient)
	float bCanFly;
};
