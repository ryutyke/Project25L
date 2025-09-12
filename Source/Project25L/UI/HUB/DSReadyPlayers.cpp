// Default
#include "UI/HUB/DSReadyPlayers.h"

// UE
#include "Components/TextBlock.h"
#include "Components/Image.h"

#include "DSLogChannels.h"

UDSReadyPlayers::UDSReadyPlayers(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, ReadyStatusText("")
{
}


void UDSReadyPlayers::UpdatePlayerCount(int CurrentPlayerCount, int MaxPlayerCount)
{
	FString FormattedText = FString::Format(*ReadyStatusText, { CurrentPlayerCount, MaxPlayerCount });

	if (IsValid(Text_Count))
	{
		Text_Count->SetText(FText::FromString(FormattedText));
	}
}
