#pragma once

// Default
#include "CoreMinimal.h"

// UE
#include "GameFramework/GameState.h"

// Game
#include "GameData/DSEnums.h"

// UHT
#include "DSLobbyGameState.generated.h"

USTRUCT(BlueprintType)
struct FJoinedPlayerInfo
{
	GENERATED_BODY()

public:
	FJoinedPlayerInfo() {}
	FJoinedPlayerInfo(int32 ID, int32 ButtonIndex, bool bIsReady)
		: PlayerID(ID)
		, CurrentButtonIndex(ButtonIndex)
		, bIsReady(bIsReady)
		{ }

	UPROPERTY()
	int32 PlayerID;
	
	UPROPERTY()
	int32 CurrentButtonIndex;
	
	UPROPERTY()
	uint8 bIsReady : 1;
};

class APlayerController;
UCLASS()
class PROJECT25L_API ADSLobbyGameState : public AGameState
{
	GENERATED_BODY()
	
public:

	ADSLobbyGameState();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	//플레이어가 입장할 때 Players를 생성해준다. -> PlayerState가 스폰될 때 호출해준다.
	void JoinPlayer(APlayerController* JoinedPlayer);
	void ReadyPlayer(int32 PlayerID);
	void UpdateSelectedButtonIndex(APlayerController* UpdatedPlayer, int32 UpdatedButtonIndex);
protected:
	UFUNCTION()
	void OnRep_PlayerInfo();
	void UpdatePlayers();
protected:
	
	UFUNCTION(Server, Reliable)
	void ServerRPC_ReadyPlayer(int32 PlayerID);
protected:
	UPROPERTY(Transient)
	uint8 bIsInitialized : 1;

	//플레이어 아이디, 플레이어 선택 캐릭터, 준비 했는가 => 플레이어가 캐릭터를 선택했는가
	UPROPERTY(Transient, ReplicatedUsing = OnRep_PlayerInfo)
	TArray<FJoinedPlayerInfo> Players;
};
