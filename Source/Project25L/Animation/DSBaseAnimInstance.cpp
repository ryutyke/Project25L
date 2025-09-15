//Default
#include "Animation/DSBaseAnimInstance.h"

//UE
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Game
#include "Character/Characters/DSCharacter.h"
#include "Components/DSFlightComponent.h"
#include "System/DSEventSystems.h"


UDSBaseAnimInstance::UDSBaseAnimInstance()
	:
	Super()
	, FlightComponent(nullptr)
	, bIsFlying(false)
	, bIsBoost(false)
	, FlightLeanX(0.f)
	, FlightLeanY(0.f)
	, HoverSpeedX(0.f)
	, HoverSpeedY(0.f)
	, HoverSpeedZ(0.f)
	, HoverMaxSpeed(0.f)
	, IKAlpha(1.0f)
	, Owner(nullptr)
	, Movement(nullptr)
	, Velocity(FVector::ZeroVector)
	, CurrentSpeed(0.f)
	, bisIdle(false)
	, bIsCrouching(false)
	, bIsFalling(false)
	, bIsJumping(false)
	, bShouldMove(false)
	, bIsDodge(false)
	, Direction(0.f)
	, MovingThreshold(3.0f)
	, JumpingThreshold(30.f)

{
	
}

void UDSBaseAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	Owner = Cast<ACharacter>(GetOwningActor());

	if (IsValid(Owner))
	{
		Movement = Owner->GetCharacterMovement();
	}


	ADSCharacter* DSCharacter = Cast<ADSCharacter>(Owner);

	if (IsValid(DSCharacter))
	{
		FlightComponent = DSCharacter->GetFlightComponent();
	}
}

void UDSBaseAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (IsValid(Movement))
	{
		Velocity = Movement->Velocity;
		CurrentSpeed = Velocity.Size2D(); 
		bisIdle = CurrentSpeed < MovingThreshold;
		bIsFalling = Movement->IsFalling();
		bIsJumping = bIsFalling && (Velocity.Z > JumpingThreshold);
		bIsCrouching = Owner->GetCharacterMovement()->IsCrouching();

		FVector Acceleration = Movement->GetCurrentAcceleration();
		bool bHasInput = !Acceleration.IsNearlyZero();
		bShouldMove = (CurrentSpeed > MovingThreshold) && bHasInput;

		/*비행 상태*/
		bIsFlying = Movement->MovementMode == EMovementMode::MOVE_Flying;
		HoverMaxSpeed = Movement->MaxFlySpeed;
	}

	ADSCharacter* DSCharacter = Cast<ADSCharacter>(Owner);

	if (IsValid(DSCharacter))
	{
		bIsDodge = DSCharacter->GetIsDodge();
		Direction = CalculateDirection(Velocity, DSCharacter->GetActorRotation());
	}

	if (IsValid(FlightComponent))
	{
		FVector2D Lean = FlightComponent->GetLean();
		FlightLeanY = Lean.Y;
		FlightLeanX = Lean.X;

		FVector RotateVec = UKismetMathLibrary::Quat_UnrotateVector(Owner->GetActorRotation().Quaternion(), Velocity);

		HoverSpeedX = UKismetMathLibrary::MapRangeClamped(RotateVec.X, -HoverMaxSpeed, HoverMaxSpeed, -1.0f, 1.0f);
		HoverSpeedY = UKismetMathLibrary::MapRangeClamped(RotateVec.Y, -HoverMaxSpeed, HoverMaxSpeed, -1.0f, 1.0f);
		HoverSpeedZ = UKismetMathLibrary::MapRangeClamped(RotateVec.Z, -HoverMaxSpeed, HoverMaxSpeed, -1.0f, 1.0f);

		float DestIKAlpha = bIsFlying == true ? 0.0f : 1.0f;

		IKAlpha = FMath::FInterpTo(IKAlpha, DestIKAlpha, DeltaSeconds, 10.f);
		bIsBoost = 200 < CurrentSpeed; /*450은 캐릭터마다 바뀔 수 있는 값*/
	}

}
