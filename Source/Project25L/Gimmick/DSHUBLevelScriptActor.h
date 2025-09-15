#pragma once

// Default
#include "CoreMinimal.h"

// UE
#include "Engine/LevelScriptActor.h"

// UHT
#include "DSHUBLevelScriptActor.generated.h"

UCLASS()
class PROJECT25L_API ADSHUBLevelScriptActor : public ALevelScriptActor
{
	GENERATED_BODY()

public:	

	ADSHUBLevelScriptActor();

protected:
	virtual void BeginPlay() override;

protected:
	void CheckSpawnedDoor();

protected:
	
	UPROPERTY(EditAnywhere, Category = "DSSettings | Max")
	int32 DoorMaxCount;
};
