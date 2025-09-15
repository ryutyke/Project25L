#pragma once

// Default
#include "CoreMinimal.h"

// UE
#include "UObject/NoExportTypes.h"

// Game
#include "Item/DSItem.h"

// UHT
#include "DSItemGrenade.generated.h"


UCLASS()
class PROJECT25L_API UDSItemGrenade : public UObject, public IDSItem
{
	GENERATED_BODY()
	
public:
	UDSItemGrenade();

	virtual void Initialize(FTableRowBase* DataTableRow) override;

	virtual void UseItem(UDSStatComponent* Stat) override;

protected:

	UPROPERTY(Transient)
	float ThrowRange;

	UPROPERTY(Transient)
	float ThrowDamage;
};
