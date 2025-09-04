#pragma once

// Default
#include "CoreMinimal.h"

// UE
#include "UObject/NoExportTypes.h"

// Game
#include "Item/DSItem.h"

// UHT
#include "DSItemVehicle.generated.h"


UCLASS()
class PROJECT25L_API UDSItemVehicle : public UObject, public IDSItem
{
	GENERATED_BODY()
	
public:
	UDSItemVehicle();

	virtual void UseItem(UDSStatComponent* Stat) override;
	virtual void Initialize(FTableRowBase* DataTableRow) override;
protected:
	UPROPERTY(Transient)
	float Duration;
	
	UPROPERTY(Transient)
	float WalkSpeed;

	UPROPERTY(Transient)
	float RunSpeed;

	UPROPERTY(Transient)
	float JumpHeight;

	UPROPERTY(Transient)
	bool bCanFly;
};
