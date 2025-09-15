#pragma once

// Default
#include "CoreMinimal.h"

// UE
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"

// Game
#include "GameData/DSEnums.h"

// UHT
#include "DSDungeonMat.generated.h"

class ACharacter;
class UStaticMeshComponent;
class UBoxComponent;
class UDSDoorData;

UCLASS()
class PROJECT25L_API ADSDungeonMat : public AActor
{
	GENERATED_BODY()
	
public:	
	ADSDungeonMat();

	void BindEvents();

	void UnbindEvents();

	FORCEINLINE EDungeonType GetDungeonDoorType() { return DungeonType; }

	void InitializeData(const FString& DoorDestination);

protected:

	UFUNCTION()
	void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void PrepareDungeonTravel();

	void TravelDungeon();

protected:

	UPROPERTY(Transient)
	FString Destination;

	UPROPERTY(Transient)
	TArray<TWeakObjectPtr<ACharacter>> OverlappedPlayers;
	
	UPROPERTY(Transient)
	FTimerHandle WaitTimer;
	
protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DSSettings | MaxPlayer")
	int32 MaxPlayers;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DSSettings | Type")
	EDungeonType DungeonType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DSSettings | Timer")
	float WaitSec;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DSSettings | Collision")
	TObjectPtr<UBoxComponent> Trigger;

	UPROPERTY(EditAnywhere, Category = "DSSettings | GameTag")
	FGameplayTag ReadyPlayerWidgetTag;
};
