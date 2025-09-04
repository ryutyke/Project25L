#pragma once

// Default
#include "CoreMinimal.h"

// UE
#include "GameFramework/PlayerController.h"

// Game
#include "GameData/DSEnums.h"

// UHT
#include "DSLobbyPlayerController.generated.h"

UCLASS()
class PROJECT25L_API ADSLobbyPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	ADSLobbyPlayerController();

	virtual void BeginPlay() override;

	void SelectedCharacterType(ECharacterType CharacterType, int32 CurrentButtonIdex, int32 PrevButtonIndex);

	void RefreshPlayerInfo(int32 CurrentButtonIndex, int32 PrevButtonIndex);

	void Ready();

protected:
	UFUNCTION(Server, Reliable)
	void ServerRPC_SelectedCharacterType(int32 CurrentButtonIdex, int32 PrevButtonIndex);

	UFUNCTION(Client, Reliable)
	void ClientRPC_SelectedCharacterType(int32 CurrentButtonIdex, int32 PrevButtonIndex);

	UFUNCTION(Server, Reliable)
	void ServerRPC_Ready();

protected:


};
