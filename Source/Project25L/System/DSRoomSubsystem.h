#pragma once

// Default
#include "CoreMinimal.h"

// UE
#include "Subsystems/GameInstanceSubsystem.h"

// Game
#include "GameData/DSEnums.h"

// UHT
#include "DSRoomSubsystem.generated.h"


UCLASS()
class PROJECT25L_API UDSRoomSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:

	UDSRoomSubsystem();
	
	static UDSRoomSubsystem* Get(UObject* Object);

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	void InitializeStage();

	void ClearStage(EDungeonType NextStage);

protected:

	UPROPERTY(Transient)
	EDungeonType CurrentStage;
};
