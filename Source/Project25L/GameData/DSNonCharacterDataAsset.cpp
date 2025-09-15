// Default
#include "DSNonCharacterDataAsset.h"

// Game
#include "DSLogChannels.h"

uint32 UDSNonCharacterDataAsset::GetKey()
{
    return Type;
}

FPrimaryAssetId UDSNonCharacterDataAsset::GetPrimaryAssetId() const
{
    return FPrimaryAssetId("DSNonCharacterDataAsset", GetFName());
}
