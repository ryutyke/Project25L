// Default
#include "Game/DSLobbyGameMode.h"

// UE
#include "Kismet/GameplayStatics.h"

// Game
#include "Game/DSLobbyGameState.h"

void ADSLobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (IsValid(NewPlayer))
	{
		ADSLobbyGameState* LobbyGameState = GetGameState<ADSLobbyGameState>();

		if (IsValid(LobbyGameState))
		{
			LobbyGameState->JoinPlayer(NewPlayer);
		}
	}
}

void ADSLobbyGameMode::StartGame()
{
	UWorld* World = GetWorld();

	check(World);

	FString StartMapPath = FString::Printf(TEXT("%s?listen"), *Path);

	World->ServerTravel(StartMapPath);
}
