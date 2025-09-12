// Defualt
#include "Components/DSFlightComponent.h"

// UE
#include "Animation/AnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"

//Game
#include "Character/Characters/DSCharacter.h"
#include "Components/DSPlayerInputComponent.h"
#include "DSCharacterMovementComponent.h"
#include "System/DSEventSystems.h"

#include "DSLogChannels.h"

UDSFlightComponent::UDSFlightComponent()
	: Super()
	, CurrentState(EFlightState::None)
	, LastVelocityRotation()
	, PreVelocityRotation()
	, Lean(0.f, 0.f)
{
	PrimaryComponentTick.bCanEverTick = true;
	bWantsInitializeComponent = true;
}

void UDSFlightComponent::InitializeComponent()
{
	Super::InitializeComponent();

	SetFlightState(EFlightState::None);
	DSEVENT_DELEGATE_BIND(OnFlightStateChanged, this, &UDSFlightComponent::SetFlightState);
}

void UDSFlightComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (CurrentState == EFlightState::Down)
	{
		ACharacter* Character = Cast<ACharacter>(GetOwner());

		if (false == IsValid(Character))
		{
			return;
		}

		UDSCharacterMovementComponent* MovementComponent = Cast< UDSCharacterMovementComponent>(Character->GetCharacterMovement());

		if (false == IsValid(MovementComponent))
		{
			return;
		}

		if (true == MovementComponent->CanLand())
		{
			SetFlightState(EFlightState::Landed);
		}
	}

	else if(CurrentState == EFlightState::Hovering)
	{
		ACharacter* Character = Cast<ACharacter>(GetOwner());

		if (false == IsValid(Character))
		{
			return;
		}
		float Velocity = Character->GetVelocity().Length();

		if (Velocity != 0.0f)
		{
			//움직이고 있다면,
			FRotator VelocityRotator = UKismetMathLibrary::MakeRotFromX(Character->GetVelocity()); //실제 향하기 위한 방향

			FRotator ActorRotator = Character->GetActorRotation();

			LastVelocityRotation = FRotator(ActorRotator.Roll, VelocityRotator.Yaw ,ActorRotator.Pitch); //속도만큼 올라감을 표현하려는 거구나?
		}
		//Lean을 업데이트 한다. => Lean 과 회전은 일치하는 느낌 Turn in place 처럼 보간이 필요하기 때문에, Tick에서 수행해주어야한다.
		FRotator UpdateVelocityRotation = LastVelocityRotation - PreVelocityRotation;

		float YawDifferentVelocity = UpdateVelocityRotation.Yaw / DeltaTime; //매 프레임마다 얼마나 달라지는 지를 확인 - Yaw
		float PitchDifferentVelocity = UpdateVelocityRotation.Pitch / DeltaTime; //매 프레임마다 얼마나 달라지는 지를 확인 - Pitch

		PreVelocityRotation = UpdateVelocityRotation;

		YawDifferentVelocity = UKismetMathLibrary::MapRangeClamped(YawDifferentVelocity, -180.f, 180.f, -1.0f, 1.0f);
		PitchDifferentVelocity = UKismetMathLibrary::MapRangeClamped(PitchDifferentVelocity, -180.f, 180.f, -1.0f, 1.0f);

		Lean.X = FMath::FInterpTo(Lean.X, YawDifferentVelocity, DeltaTime, 5.0f);
		Lean.Y = FMath::FInterpTo(Lean.Y, PitchDifferentVelocity, DeltaTime, 5.0f);
	}

}

void UDSFlightComponent::ServerRPC_ChangeFlightMode_Implementation(EFlightState FlightMode)
{
	//Replicate 되어진다.
	CurrentState = FlightMode;
}

void UDSFlightComponent::MoveUpDown(EFlightState Direction)
{
	ACharacter* Character = Cast<ACharacter>(GetOwner());

	if (false == IsValid(Character))
	{
		return;
	}

	if (FlightVerticalImpulse.Contains(Direction))
	{
		Character->AddMovementInput(FVector(0.f, 0.f, FlightVerticalImpulse[Direction]));
	}
}

void UDSFlightComponent::HandleFlightStateChanged(EFlightState FlightMode)
{
	ADSCharacter* Character = Cast<ADSCharacter>(GetOwner());

	if (false == IsValid(Character))
	{
		return;
	}

	bool bIsFlighted = FlightMode != EFlightState::Landed;
	
	DSEVENT_DELEGATE_INVOKE(Character->OnMovementModeChanged, bIsFlighted, false);
}

void UDSFlightComponent::SetFlightState(EFlightState NewState)
{
	// Flight 상태에 따라서 변화한다.
	switch (NewState)
	{
	case EFlightState::Up:
	case EFlightState::Down:
		MoveUpDown(NewState);
		break;
	case EFlightState::Flighted:
	case EFlightState::Landed:
		HandleFlightStateChanged(NewState);
		break;
	}

	//Down를 제외한 모든 경우의 수는 나는 경우의 수이다. (Down은 Landed로 변경 가능하기 때문이다.)
	EFlightState UpdatedFlightState = NewState != EFlightState::Down? EFlightState::Hovering : NewState;

	if (CurrentState != UpdatedFlightState)
	{
		ServerRPC_ChangeFlightMode(UpdatedFlightState);
	}
}

void UDSFlightComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UDSFlightComponent, CurrentState);
}
