#pragma once

// Default
#include "CoreMinimal.h"

// UE
#include "Components/ActorComponent.h"

// Game
#include "GameData/DSEnums.h"

// UHT
#include "DSFlightComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECT25L_API UDSFlightComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UDSFlightComponent();

	FORCEINLINE FVector2D GetLean() { return Lean; }
	
	// Delegate
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnFlightStateChanged, EFlightState);
	FOnFlightStateChanged OnFlightStateChanged;

protected:
	virtual void InitializeComponent() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

protected:
	void SetFlightState(EFlightState NewState);
	void MoveUpDown(EFlightState Direction);

	void HandleFlightStateChanged(EFlightState FlightMode);
protected:
	UFUNCTION(Server, Reliable)
	void ServerRPC_ChangeFlightMode(EFlightState FlightMode);
	
private:

	UPROPERTY(Transient, Replicated)
	EFlightState CurrentState;

	UPROPERTY(Transient)
	FRotator LastVelocityRotation;
	
	UPROPERTY(Transient)
	FRotator PreVelocityRotation;
	
	UPROPERTY(Transient)
	FVector2D Lean;

	UPROPERTY(EditAnywhere, Category = "DSSettings | Direction", Meta = (AllowPrivateAccess = "true"))
	TMap<EFlightState, float> FlightVerticalImpulse;
};
