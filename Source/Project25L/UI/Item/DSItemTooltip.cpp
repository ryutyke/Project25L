// Default
#include "UI/Item/DSItemTooltip.h"

// UE
#include "Components/TextBlock.h"

// Game
#include "GameData/Items/DSItemData.h"
#include "System/DSGameDataSubsystem.h"

UDSItemTooltip::UDSItemTooltip(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UDSItemTooltip::SetText(int32 ItemID)
{
	UDSGameDataSubsystem* DataManager = UDSGameDataSubsystem::Get(this);

	check(DataManager);

	FDSItemData* ItemData = static_cast<FDSItemData*>(DataManager->GetDataRowByID(EDataTableType::ItemData, ItemID));

	if (nullptr != ItemData)
	{
		if (IsValid(Text_ItemName))
		{
			Text_ItemName->SetText(ItemData->ItemName);
		}

		if (IsValid(Text_Tooltip))
		{
			Text_Tooltip->SetText(ItemData->Tooltip);
		}

		if (IsValid(Text_Stat))
		{
			Text_Stat->SetText(ItemData->Description);
		}
	}
}
