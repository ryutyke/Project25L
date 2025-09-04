// Default
#include "DSReadyBoardPanel.h"

// UE
#include "GameFramework/GameState.h"

// Game
#include "DSLogChannels.h"
#include "DSReadyBoardEntry.h"

UDSReadyBoardPanel::UDSReadyBoardPanel(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
}

void UDSReadyBoardPanel::NativeConstruct()
{
    Super::NativeConstruct();
}

void UDSReadyBoardPanel::InitializeWidget()
{
    APlayerController* PlayerController = GetOwningPlayer();

    if(false == IsValid(PlayerController))
    {
        return;
    }

    UWorld* World = PlayerController->GetWorld();
    check(World);

	AGameStateBase* GameState = World->GetGameState();
    if (false == IsValid(GameState))
    {
       return;
    }

    const int32 PlayerCount = GameState->PlayerArray.Num();
    if(PlayerCount <= ReadyBoardEntryArray.Num())
    {
        return;
    }
    
    for(int i =0; i<PlayerCount; ++i)
    {
        UDSReadyBoardEntry* Entry = NewObject<UDSReadyBoardEntry>();
        ReadyBoardEntryArray.Push(Entry);
    }
}
