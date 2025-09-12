#pragma once

// Defualt
#include "CoreMinimal.h"

// UE
#include "Components/ActorComponent.h"

// UHT
#include "DSChatComponent.generated.h"


DECLARE_MULTICAST_DELEGATE_TwoParams(FOnChatReceived, const ECharacterType /*SenderType*/, const FText& /*Message*/);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECT25L_API UDSChatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UDSChatComponent();

public:
	void SendChatMessageToServer(const FText& Message);
	FOnChatReceived OnChatReceived;
protected:
	virtual void BeginPlay() override;

	UFUNCTION(Server, Reliable)
	void ServerRPC_SendChatMessage(const FText& Message);

	UFUNCTION(Client, Unreliable)
	void ClientRPC_ReceiveChatMessage(const ECharacterType SenderType, const FText& Message);		
};
