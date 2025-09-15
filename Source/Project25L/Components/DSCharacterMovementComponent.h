#pragma once
//Default
#include "CoreMinimal.h"

//UE
#include "GameFramework/CharacterMovementComponent.h"

//Game
#include "GameData/DSEnums.h"

//UHT
#include "DSCharacterMovementComponent.generated.h"

class FDSSavedMove_Character : public FSavedMove_Character
{
	
public:
	virtual void Clear() override;
	virtual void SetInitialPosition(ACharacter* Character) override;
	virtual uint8 GetCompressedFlags() const override;

	uint8 bChangeSpeed : 1;
	ESpeedType SavedSpeedType;
};


class FDSNetworkPredictionData_Client_Character : public FNetworkPredictionData_Client_Character //상속받아서 구현 
{
public:
	FDSNetworkPredictionData_Client_Character(const UCharacterMovementComponent& ClientMovement);

	virtual FSavedMovePtr AllocateNewMove() override;
};


UCLASS()
class PROJECT25L_API UDSCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
	
public:
	UDSCharacterMovementComponent();
	void SetSpeedCommand(ESpeedType TartgetSpeed);
	virtual bool IsMovingOnGround() const override;
	bool CanLand() const;

	void HandleMovementModeChanged(bool bCanFly, bool bIsCompleted);

	UFUNCTION(Server, Reliable)
	void ServerRPC_OnMovementModeChanged(EMovementMode InMovementMode);

	virtual void ServerMove_PerformMovement(const FCharacterNetworkMoveData& MoveData) override;

protected:
	virtual class FNetworkPredictionData_Client* GetPredictionData_Client() const override;
	virtual void OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity) override;
	virtual float GetMaxSpeed() const override;
	virtual void UpdateFromCompressedFlags(uint8 Flags);
	void ApplyFloatingEffect(float DeltaSeconds);

public:
	UPROPERTY(Transient)
	uint8 bChangeSpeed : 1;
	
	UPROPERTY(Transient)
	ESpeedType CurrentSpeedType;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "DSSettings | Movement")
	TMap<ESpeedType, float> SpeedMode;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "DSSettings | Movement")
	TMap<ESpeedType, float> FlightSpeedMode;

	UPROPERTY(Transient)
	float GroundCheckThreshold;

};
