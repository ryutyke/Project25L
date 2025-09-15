// Default
#include "Player/DSPlayerState.h"

// UE
#include "EngineUtils.h"
#include "GameFramework/GameState.h"
#include "Net/UnrealNetwork.h"

// Game
#include "DSLogChannels.h"
#include "Character/Characters/DSCharacter.h"
#include "GameData/GameplayTag/DSGameplayTags.h"
#include "Player/DSPlayerController.h"
#include "System/DSEventSystems.h"
#include "System/DSLocalPlayerSubsystem.h"
#include "UI/Game/Player/DSPlayerInfo.h"

ADSPlayerState::ADSPlayerState()
	: Super()
	, CharacterType(ECharacterType::None)
{
}

void ADSPlayerState::ServerRPC_SetCharacterType_Implementation(ECharacterType ChangedCharacterType)
{
	CharacterType = ChangedCharacterType;

	ADSPlayerController* PlayerController = Cast<ADSPlayerController>(GetPlayerController());

	if (IsValid(PlayerController))
	{
		PlayerController->SpawnInitCharacter(ChangedCharacterType);
	}
}

void ADSPlayerState::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PlayerContoller = GetPlayerController();

	if (IsValid(PlayerContoller) && PlayerContoller->IsLocalController())
	{
		UDSLocalPlayerSubsystem* Subsystem = UDSLocalPlayerSubsystem::Get(this);
		check(Subsystem);

		CharacterType = Subsystem->GetCharacterType();

		if (CharacterType == ECharacterType::None)
		{
			CharacterType = TempCharacterType;
		}
		ServerRPC_SetCharacterType(CharacterType);
	}
}

void ADSPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADSPlayerState, CharacterType);
}
