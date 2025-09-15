#pragma once

// Default
#include "CoreMinimal.h"

// UE
#include "GameFramework/PlayerState.h"

// Game
#include "GameData/DSEnums.h"

// UHT
#include "DSPlayerState.generated.h"


UCLASS()
class PROJECT25L_API ADSPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	ADSPlayerState();

	DECLARE_MULTICAST_DELEGATE_TwoParams(FOnCharacterTypeChanged, ECharacterType, FText);
	FOnCharacterTypeChanged OnCharacterTypeChanged;

	ECharacterType GetCharacterType() { return CharacterType; }
protected:
	UFUNCTION(Server, Reliable)
	void ServerRPC_SetCharacterType(ECharacterType ChangedCharacterType);

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	
	UPROPERTY(EditAnywhere, Category = "DSSettings | Temp")
	ECharacterType TempCharacterType;

	UPROPERTY(Transient, Replicated)
	ECharacterType CharacterType;
};
