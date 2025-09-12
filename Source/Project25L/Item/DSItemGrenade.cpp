// Default
#include "Item/DSItemGrenade.h"

// Game
#include "DSLogChannels.h"
#include "GameData/Items/DSItemStatData.h"

UDSItemGrenade::UDSItemGrenade()
	: Super()
	, ThrowRange(0)
	, ThrowDamage(0)
{
}

void UDSItemGrenade::Initialize(FTableRowBase* DataTableRow)
{
	FDSItemStatData* ItemData = static_cast<FDSItemStatData*>(DataTableRow);

	if (nullptr != ItemData)
	{
	}
}

void UDSItemGrenade::UseItem(UDSStatComponent* Stat)
{
	// 아이템 사용에 대한 정의 합니다.
	DS_LOG(DSItemLog, Log, TEXT("Use Item Grenade"));
}
