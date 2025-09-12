#pragma once
//Default
#include "CoreMinimal.h"

//UE
#include "Engine/GameInstance.h"

//UHT
#include "DSGameInstance.generated.h"

class UDSGameUtils;
class UDSEventSystems;

UCLASS()
class PROJECT25L_API UDSGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UDSGameInstance();

	virtual void Init() override;

	virtual void Shutdown() override;

public:
	UPROPERTY(Transient)
	TObjectPtr<UDSEventSystems> EventSystem;

	UPROPERTY(Transient)
	TObjectPtr<UDSGameUtils> GameUtils;
};
