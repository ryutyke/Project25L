#pragma once

// Default
#include "CoreMinimal.h"

// UE
#include "GameFramework/GameMode.h"

// UHT
#include "DSLobbyGameMode.generated.h"

UCLASS()
class PROJECT25L_API ADSLobbyGameMode : public AGameMode
{
	GENERATED_BODY()

public:

	virtual void PostLogin(APlayerController* NewPlayer) override;
	void StartGame();

protected:

	UPROPERTY(EditAnywhere, Category = "DSSettings | Path")
	FString Path;

};
