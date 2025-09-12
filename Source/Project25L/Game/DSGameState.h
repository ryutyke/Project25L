#pragma once

// Default
#include "CoreMinimal.h"

// UE
#include "GameFramework/GameState.h"

// UHT
#include "DSGameState.generated.h"

UCLASS()
class PROJECT25L_API ADSGameState : public AGameState
{
	GENERATED_BODY()
	
public:
	ADSGameState();

	int32 GetMaxPlayerCount() { return MaxPlayerCount; }
protected:
	UFUNCTION()
	void OnRep_SpawnedPlayerCount();

	void CheckSpawnedCharacter();

protected:
	virtual void AddPlayerState(APlayerState* PlayerState) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

protected:
	UPROPERTY(Transient)
	uint8 bIsSpawned;

	UPROPERTY(EditAnywhere, Category = "DSSettings | MaxCount")
	int32 MaxPlayerCount;

	UPROPERTY(Transient, ReplicatedUsing = OnRep_SpawnedPlayerCount)
	int32 SpawnedPlayerCount;

};
