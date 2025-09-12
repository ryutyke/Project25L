#pragma once
// Default
#include "CoreMinimal.h"

// UE
#include "GameFramework/PlayerController.h"

// Game
#include "GameData/DSEnums.h"

// UHT
#include "DSPlayerController.generated.h"

class UDSPlayerInputComponent;
class UDSChatComponent;

UCLASS()
class PROJECT25L_API ADSPlayerController : public APlayerController
{
	GENERATED_BODY()
	
	
public:
	ADSPlayerController();

public:
	ECharacterType GetCharacterType();
	void SetUIFocusMode();
	void SetGameFocusMode();
	void SpawnInitCharacter(ECharacterType CharacterType);
	UDSChatComponent* GetChatComponent() const { return DSChatComponent;}
	virtual void OnRep_Pawn() override;
protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* aPawn) override;

public:
	/*Cheat*/
	UFUNCTION(Server, Unreliable)
	void ServerRPC_Cheat(const FString& Message);

	UFUNCTION(Server, Unreliable)
	void ServerRPC_CheatAll(const FString& Message);


protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DSSettings | Chat", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDSChatComponent> DSChatComponent;
};
