#pragma once

// Default
#include "CoreMinimal.h"

// UE
#include "GameFramework/GameMode.h"

// Game
#include "GameData/DSEnums.h"

// UHT
#include "DSGameMode.generated.h"

UCLASS()
class PROJECT25L_API ADSGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:

	virtual void PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;
	virtual void PostLogin(APlayerController* NewPlayer) override;

	APawn* SpawnCharacter(APlayerController* InPlayerController, ECharacterType CharacterType);
};
