
// Default
#include "Item/DSItemAccessory.h"

// Game
#include "DSLogChannels.h"
#include "GameData/Items/DSItemStatData.h"

UDSItemAccessory::UDSItemAccessory()
	: Super()
	, Duration(0)
{
}

void UDSItemAccessory::Initialize(FTableRowBase* DataTableRow)
{
	FDSItemStatData* ItemData = static_cast<FDSItemStatData*>(DataTableRow);

	if (nullptr != ItemData)
	{
	
	}
}

void UDSItemAccessory::UseItem(UDSStatComponent* Stat)
{
	// 아이템 사용에 대한 정의 합니다.
	DS_LOG(DSItemLog, Log, TEXT("Use Item Accessory"));
}
