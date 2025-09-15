#pragma once
//Default
#include "CoreMinimal.h"

//UE
#include "Animation/AnimInstance.h"

//Game
#include "GameData/DSEnums.h"

//UHT
#include "DSBaseAnimInstance.generated.h"

class ACharacter;
class UCharacterMovementComponent;
class UDSFlightComponent;

UCLASS()
class PROJECT25L_API UDSBaseAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	UDSBaseAnimInstance();

protected:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override; 

protected:
	/*비행 상태*/
	UPROPERTY(Transient, BlueprintReadOnly)
	TObjectPtr<UDSFlightComponent> FlightComponent;

	UPROPERTY(Transient, BlueprintReadOnly)
	uint8 bIsFlying : 1;

	UPROPERTY(Transient, BlueprintReadOnly)
	uint8 bIsBoost : 1;

	UPROPERTY(Transient, BlueprintReadOnly)
	float FlightLeanX;

	UPROPERTY(Transient, BlueprintReadOnly)
	float FlightLeanY;

	UPROPERTY(Transient, BlueprintReadOnly)
	float HoverSpeedX;

	UPROPERTY(Transient, BlueprintReadOnly)
	float HoverSpeedY;

	UPROPERTY(Transient, BlueprintReadOnly)
	float HoverSpeedZ;

	UPROPERTY(Transient, BlueprintReadOnly)
	float HoverMaxSpeed;

	UPROPERTY(Transient, BlueprintReadOnly)
	float IKAlpha;

protected:
	/*일반 상태*/

	UPROPERTY(Transient, BlueprintReadOnly)
	TObjectPtr<ACharacter> Owner;

	UPROPERTY(Transient, BlueprintReadOnly)
	TObjectPtr<UCharacterMovementComponent> Movement;

	UPROPERTY(Transient, BlueprintReadOnly)
	FVector Velocity;

	UPROPERTY(Transient, BlueprintReadOnly)
	float CurrentSpeed;

	UPROPERTY(Transient, BlueprintReadOnly)
	uint8 bisIdle : 1;

	UPROPERTY(Transient, BlueprintReadOnly)
	uint8 bIsCrouching : 1;

	UPROPERTY(Transient, BlueprintReadOnly)
	uint8 bIsFalling : 1;

	UPROPERTY(Transient, BlueprintReadOnly)
	uint8 bIsJumping : 1;

	UPROPERTY(Transient, BlueprintReadOnly)
	uint8 bShouldMove : 1;

	UPROPERTY(Transient, BlueprintReadOnly)
	uint8 bIsDodge : 1;

	UPROPERTY(Transient, BlueprintReadOnly)
	float Direction;

	UPROPERTY(EditAnywhere, Category = "DSSettings | Threshold")
	float MovingThreshold;

	UPROPERTY(EditAnywhere, Category = "DSSettings | Threshold")
	float JumpingThreshold;
};
