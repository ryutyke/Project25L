// Default
#include "Player/DSLobbyPlayerController.h"

// UE
#include "GameFramework/PlayerState.h"

// Game
#include "Game/DSLobbyGameState.h"
#include "GameData/GameplayTag/DSGameplayTags.h"
#include "System/DSLocalPlayerSubsystem.h"
#include "System/DSUIManagerSubsystem.h"
#include "UI/Base/DSWidgetLayer.h"
#include "UI/Menu/DSMenu.h"

#include "DSLogChannels.h"

ADSLobbyPlayerController::ADSLobbyPlayerController()
	: Super()
{
}

void ADSLobbyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	bShowMouseCursor = true;

	FInputModeUIOnly InputMode;
	SetInputMode(InputMode);
}

void ADSLobbyPlayerController::SelectedCharacterType(ECharacterType CharacterType, int32 CurrentButtonIndex, int32 PrevButtonIndex)
{
	UDSLocalPlayerSubsystem* LocalPlayerSubsystem = UDSLocalPlayerSubsystem::Get(this);

	check(LocalPlayerSubsystem);

	LocalPlayerSubsystem->SetCharacterType(CharacterType);
	
	ServerRPC_SelectedCharacterType(CurrentButtonIndex, PrevButtonIndex);
	DS_LOG(DSUILog, Log, TEXT(" CurrentButtonIndex %d PrevButtonIndex %d %s"), CurrentButtonIndex, PrevButtonIndex, *UEnum::GetValueAsString(CharacterType));
}

void ADSLobbyPlayerController::RefreshPlayerInfo(int32 CurrentButtonIndex, int32 PrevButtonIndex)
{
	UDSUIManagerSubsystem* UIManagerSubsystem = UDSUIManagerSubsystem::Get(this);

	check(UIManagerSubsystem);

	UDSMenu* MenuWidget = Cast<UDSMenu>(UIManagerSubsystem->GetTopLayer(FDSTags::GetDSTags().UI_Layer_Persistent_Main));

	if (IsValid(MenuWidget))
	{
		DS_LOG(DSUILog, Log, TEXT(" CurrentButtonIndex %d PrevButtonIndex %d"), CurrentButtonIndex, PrevButtonIndex);
		//다른 플레이어가 선택해서, 현재 플레이어는 선택하지 못함.
		MenuWidget->UpdateButtonStates(CurrentButtonIndex, PrevButtonIndex);
	}
}

void ADSLobbyPlayerController::Ready()
{
	APlayerState* LocalPlayerState = GetPlayerState<APlayerState>();
	
	if (IsValid(LocalPlayerState))
	{
		DS_LOG(DSUILog, Log, TEXT("Player Ready %d"), LocalPlayerState->GetPlayerId());
	}

	ServerRPC_Ready();
}

void ADSLobbyPlayerController::ServerRPC_Ready_Implementation()
{
	APlayerState* LocalPlayerState = GetPlayerState<APlayerState>();

	if (false == IsValid(LocalPlayerState))
	{
		return;
	}
	
	DS_LOG(DSUILog, Log, TEXT("Player Ready %d"), LocalPlayerState->GetPlayerId());
	
	UWorld* World = GetWorld();
	check(World);

	//게임 상태를 전달한다.
	ADSLobbyGameState* LobbyGameState = World->GetGameState<ADSLobbyGameState>();

	if (IsValid(LobbyGameState))
	{
		//레디로 확정을 알려준다.
		LobbyGameState->ReadyPlayer(LocalPlayerState->GetPlayerId());
	}
}

void ADSLobbyPlayerController::ServerRPC_SelectedCharacterType_Implementation(int32 CurrentButtonIndex, int32 PrevButtonIndex)
{
	//나를 제외한 나머지 두 명한테 전달한다.
	UWorld* World = GetWorld();

	check(World);

	for (FConstPlayerControllerIterator Iterator = World->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		ADSLobbyPlayerController* PlayerController = Cast< ADSLobbyPlayerController>(Iterator->Get());

		if (IsValid(PlayerController) && this != PlayerController)
		{
			DS_LOG(DSUILog, Log, TEXT(" CurrentButtonIndex %d PrevButtonIndex %d"), CurrentButtonIndex, PrevButtonIndex);
			PlayerController->ClientRPC_SelectedCharacterType(CurrentButtonIndex, PrevButtonIndex);
		}
	}

	ADSLobbyGameState* LobbyGameState = World->GetGameState<ADSLobbyGameState>();

	if (IsValid(LobbyGameState))
	{
		LobbyGameState->UpdateSelectedButtonIndex(this, CurrentButtonIndex);
	}
}

void ADSLobbyPlayerController::ClientRPC_SelectedCharacterType_Implementation(int32 CurrentButtonIndex, int32 PrevButtonIndex)
{
	RefreshPlayerInfo(CurrentButtonIndex, PrevButtonIndex);
}
