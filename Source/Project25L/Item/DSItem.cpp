// Default
#include "Item/DSItem.h"

EItemType IDSItem::ConvertToItemType(int32 ItemID)
{
	EItemType ItemType = EItemType::None;

	int Dist = (static_cast<int32>(EItemType::Vehicle) - static_cast<int32>(EItemType::None));

	for (int32 ItemTypeIdx = static_cast<int32>(EItemType::None); ItemTypeIdx <= static_cast<int32>(EItemType::Max); ItemTypeIdx += Dist)
	{
		int DivIdx = ItemTypeIdx / ItemID;

		// 1로 딱 떨어지는 구간을 확인한다.
		if (DivIdx >= 1)
		{
			ItemType = static_cast<EItemType>(ItemTypeIdx - Dist);
			break;
		}
	}
	
	return ItemType;
}

EDataTableType IDSItem::ConvertToItemDataType(EItemType ItemType)
{

	switch (ItemType)
	{
	case EItemType::Vehicle:
		return EDataTableType::ItemVehicleData;
	case EItemType::Potion:
		return EDataTableType::ItemPotionData;
	case EItemType::Grenade:
		return EDataTableType::ItemGrenadeData;
	case EItemType::Accessory:
		return EDataTableType::ItemAccessoryData;
	}

	return EDataTableType();
}
