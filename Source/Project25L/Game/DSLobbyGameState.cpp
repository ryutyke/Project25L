// Default
#include "Game/DSLobbyGameState.h"

// UE
#include "EngineUtils.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/PlayerState.h"

// Game
#include "Player/DSLobbyPlayerController.h"
#include "Game/DSLobbyGameMode.h"

ADSLobbyGameState::ADSLobbyGameState()
	: Super()
	, bIsInitialized(false)
	, Players()
{
}

void ADSLobbyGameState::JoinPlayer(APlayerController* JoinedPlayer)
{
	if (false == IsValid(JoinedPlayer))
	{
		return;
	}

	APlayerState* PlayerState = JoinedPlayer->GetPlayerState<APlayerState>();

	if (false == IsValid(PlayerState))
	{
		return;
	}

	//Players 없다면, 생성한다.
	
	bool bIsExisted = false;
	//있다면 아무행위하지 않는다.
	ADSLobbyPlayerController* LobbyPlayerController = Cast<ADSLobbyPlayerController>(JoinedPlayer);

	if (IsValid(LobbyPlayerController))
	{
		int32 JoinedPlayerIndex = 0;
		for (; JoinedPlayerIndex < Players.Num(); JoinedPlayerIndex++)
		{
			//들어왔을 때 이미 버튼을 누른 사람이 있다면,
			if (Players[JoinedPlayerIndex].CurrentButtonIndex != -1)
			{
				//현재 플레이어에게 그 버튼을 누르면 안된다고 전달해주어야 한다.
				LobbyPlayerController->ClientRPC_SelectedCharacterType_Implementation(Players[JoinedPlayerIndex].CurrentButtonIndex, -1);
			}

			if (Players[JoinedPlayerIndex].PlayerID == PlayerState->GetPlayerId())
			{
				bIsExisted = true;
			}
		}

		if (false == bIsExisted)
		{
			//없음.
			Players.Add({ PlayerState->GetPlayerId(), -1, false });
		}
	}
}

void ADSLobbyGameState::OnRep_PlayerInfo()
{
	//OnRep이 변경되어지면, 
	//현재 처음 들어왔다면, 현재 Player 정보를 반영해서 UI를 업데이트 해준다.
	//버튼 누름과 복구는 RPC에 의해 이루어지기 때문이 이와 같은 함수를 설정해 준다.

	if (false == bIsInitialized)
	{
		//Menu 띄우고, UI를 업데이트 해준다.
		UpdatePlayers();
		bIsInitialized = true;
	}
}

void ADSLobbyGameState::UpdatePlayers()
{
	UWorld* World = GetWorld();
	check(World);

	//로컬 컴퓨터 상의 첫 번째 플레이어 컨트롤러를 가지고 온다.
	APlayerController* PlayerController = World->GetFirstPlayerController();

	if (IsValid(PlayerController))
	{
		
		// 나 자신을 제외한 나머지가 선택한 캐릭터를 반영해준다.
		// 아래와 같이 짠 이유는 플레이어가 나중에 들어올 수도 있기 때문이다.
		for (FConstPlayerControllerIterator Iterator = World->GetPlayerControllerIterator(); Iterator; ++Iterator)
		{
			ADSLobbyPlayerController* TargetPlayerController = Cast< ADSLobbyPlayerController>(Iterator->Get());
			for (int32 JoinedPlayerIndex = 0; JoinedPlayerIndex < Players.Num(); JoinedPlayerIndex++)
			{
				if (Players[JoinedPlayerIndex].CurrentButtonIndex != -1)
				{
					//-1의 의미는 고르지 않음을 의미함.
					TargetPlayerController->RefreshPlayerInfo(Players[JoinedPlayerIndex].CurrentButtonIndex, -1);
				}
			}
		}
	}
}

void ADSLobbyGameState::ReadyPlayer(int32 PlayerID)
{
	ServerRPC_ReadyPlayer(PlayerID);
}

void ADSLobbyGameState::UpdateSelectedButtonIndex(APlayerController* UpdatedPlayer, int32 UpdatedButtonIndex)
{
	if (false == IsValid(UpdatedPlayer))
	{
		return;
	}

	APlayerState* PlayerState = UpdatedPlayer->GetPlayerState<APlayerState>();

	if (false == IsValid(PlayerState))
	{
		return;
	}

	for (int32 JoinedPlayerIndex = 0; JoinedPlayerIndex < Players.Num(); JoinedPlayerIndex++)
	{
		if (Players[JoinedPlayerIndex].PlayerID == PlayerState->GetPlayerId())
		{
			Players[JoinedPlayerIndex].CurrentButtonIndex = UpdatedButtonIndex;
		}
	}
}

void ADSLobbyGameState::ServerRPC_ReadyPlayer_Implementation(int32 PlayerID)
{
	bool bIsReady = true;
	int32 JoinedPlayerIndex = 0;
	for (; JoinedPlayerIndex < Players.Num(); JoinedPlayerIndex++)
	{
		//아이디가 같다면, 해당 플레이어를 준비시킨다.		
		if (Players[JoinedPlayerIndex].PlayerID == PlayerID)
		{
			Players[JoinedPlayerIndex].bIsReady = true;
		}
		
		//한명이라도 레디를 안했다면, 준비안했음을 알려주어야한다.
		if (false == Players[JoinedPlayerIndex].bIsReady)
		{
			bIsReady = false;
		}

	}

	if (bIsReady)
	{
		//게임 모드에게 게임을 시작할 모든 준비가 되었음을 알려준다.
		UWorld* World = GetWorld();

		check(World);

		ADSLobbyGameMode* GameMode = Cast<ADSLobbyGameMode>(World->GetAuthGameMode());

		//모든 준비가 다 됐어를 알려준다.
		if (IsValid(GameMode))
		{
			GameMode->StartGame();
		}
	}
}

void ADSLobbyGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADSLobbyGameState, Players);
}