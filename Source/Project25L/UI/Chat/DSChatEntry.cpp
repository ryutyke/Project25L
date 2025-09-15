// Defualt
#include "DSChatEntry.h"

// UE
#include "Components/Image.h"
#include "Components/TextBlock.h"

// Game
#include "GameData/Chat/DSChatData.h"
#include "GameData/DSCharacterDataAsset.h"
#include "System/DSGameDataSubsystem.h"

void UDSChatEntry::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	UDSChatData* ChatData = Cast<UDSChatData>(ListItemObject);

	if(false == IsValid(ChatData))
	{
		return;
	}

	if(false == IsValid(CharacterIcon)) 
	{
		return;
	}

	UDSGameDataSubsystem* DataSubsystem = UDSGameDataSubsystem::Get(this);
	check(DataSubsystem);
	const UDSCharacterDataAsset* DataAsset = DataSubsystem->GetDataAssetByType<ECharacterType, UDSCharacterDataAsset>(ChatData->CharacterType);

	if (true == IsValid(DataAsset))
	{
		if(DataAsset->PlayerIcon.IsValid())
		{
			CharacterIcon->SetBrushFromSoftTexture(DataAsset->PlayerIcon.LoadSynchronous());
		}
	}

	if(true == IsValid(ChatContentText))
	{
		ChatContentText->SetText(ChatData->ChatContent);
	}
}
