// Default
#include "GameData/DSCharacterDataAsset.h"

// UE

// Game


uint32 UDSCharacterDataAsset::GetKey()
{
	uint32 EnumAsUint32 = static_cast<uint32>(Type);
	return EnumAsUint32;
}

FPrimaryAssetId UDSCharacterDataAsset::GetPrimaryAssetId() const
{
	return FPrimaryAssetId("DSCharacterDataAsset", GetFName());
}
