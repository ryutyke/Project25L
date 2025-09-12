//Default
#include "Player/DSPlayerController.h"

//UE
#include "EngineUtils.h"
#include "Net/UnrealNetwork.h"

//Game
#include "Components/DSChatComponent.h"
#include "DSLogChannels.h"
#include "Game/DSGameMode.h"
#include "HUD/DSHUD.h"
#include "Player/DSCheatManager.h"
#include "Player/DSPlayerState.h"
#include "System/DSEventSystems.h"
#include "System/DSLocalPlayerSubsystem.h"
#include "System/DSUIManagerSubsystem.h"
#include "UI/Base/DSWidgetLayer.h"
#include "GameData/GameplayTag/DSGameplayTags.h"

ADSPlayerController::ADSPlayerController() :
	Super()
{
#if USING_CHEAT_MANAGER
	CheatClass = UDSCheatManager::StaticClass();
#endif

	DSChatComponent = CreateDefaultSubobject<UDSChatComponent>(TEXT("ChatComponent"));
}

ECharacterType ADSPlayerController::GetCharacterType()
{
	ADSPlayerState* LocalPlayerState = GetPlayerState<ADSPlayerState>();

	if (IsValid(LocalPlayerState))
	{
		LocalPlayerState->GetCharacterType();
	}
	return ECharacterType::Girl;
}

void ADSPlayerController::SetUIFocusMode()
{
	bShowMouseCursor = true;

	FInputModeGameAndUI InputMode;
	SetInputMode(InputMode);
}

void ADSPlayerController::SetGameFocusMode()
{
	bShowMouseCursor = false;

	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);
}

void ADSPlayerController::SpawnInitCharacter(ECharacterType ChangedCharacterType)
{
	UWorld* World = GetWorld();
	check(World);

	ADSGameMode* GameMode = Cast<ADSGameMode>(World->GetAuthGameMode());

	if (IsValid(GameMode))
	{
		APawn* NewPawn = GameMode->SpawnCharacter(this, ChangedCharacterType);

		if (IsValid(NewPawn))
		{
			Possess(NewPawn);
		}
	}	
}

void ADSPlayerController::OnRep_Pawn()
{
	Super::OnRep_Pawn();


}



void ADSPlayerController::BeginPlay()
{
	Super::BeginPlay();

	SetGameFocusMode();
}

void ADSPlayerController::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);
}

void ADSPlayerController::ServerRPC_Cheat_Implementation(const FString& Message)
{
	if (CheatManager)
	{
		DS_NETLOG(DSNetLog, Warning, TEXT("ServerCheat: %s"), *Message);
		ClientMessage(ConsoleCommand(Message));
	}
}

void ADSPlayerController::ServerRPC_CheatAll_Implementation(const FString& Message)
{
	if (CheatManager)
	{
		UWorld* World = GetWorld();

		check(World);

		for (TActorIterator<ADSPlayerController> It(World); It; ++It)
		{
			ADSPlayerController* PC = (*It);

			if (PC)
			{
				DS_NETLOG(DSNetLog, Warning, TEXT("ServerCheatAll: %s"), *Message);
				PC->ClientMessage(PC->ConsoleCommand(Message));
			}
		}
	}
}

