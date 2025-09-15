#pragma once

// Default
#include "CoreMinimal.h"

// UE
#include "UObject/Interface.h"

// Game
#include "GameData/DSEnums.h"

// UHT
#include "DSItem.generated.h"

class UDSStatComponent;

UINTERFACE(MinimalAPI)
class UDSItem : public UInterface
{
	GENERATED_BODY()
};

class PROJECT25L_API IDSItem
{
	GENERATED_BODY()

public:

	virtual void UseItem(UDSStatComponent* Stat) = 0;
	virtual void Initialize(FTableRowBase* DataTableRow) = 0;
	static EItemType ConvertToItemType(int32 ItemType);

	static EDataTableType ConvertToItemDataType(EItemType ItemType);
};
