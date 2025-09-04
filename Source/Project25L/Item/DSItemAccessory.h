#pragma once

// Default
#include "CoreMinimal.h"

// UE
#include "UObject/NoExportTypes.h"

// Game
#include "Item/DSItem.h"

// UHT
#include "DSItemAccessory.generated.h"


UCLASS()
class PROJECT25L_API UDSItemAccessory : public UObject, public IDSItem
{
	GENERATED_BODY()
	
public:
	UDSItemAccessory();

	virtual void Initialize(FTableRowBase* DataTableRow) override;

	virtual void UseItem(UDSStatComponent* Stat) override;

protected:

	UPROPERTY(Transient)
	float Duration;

};
