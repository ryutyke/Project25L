//Default
#include "Components/DSCharacterMovementComponent.h"

//UE

//Game
#include "Components/DSFlightComponent.h"
#include "Character/Characters/DSCharacter.h"
#include "DSLogChannels.h"
#include "Components/CapsuleComponent.h"


UDSCharacterMovementComponent::UDSCharacterMovementComponent()
	: Super()
	, bChangeSpeed(false)
	, CurrentSpeedType(ESpeedType::None)
	, GroundCheckThreshold(70.0f)

{
}

void UDSCharacterMovementComponent::SetSpeedCommand(ESpeedType TargetWalkSpeed)
{
	bChangeSpeed = true;
	CurrentSpeedType = TargetWalkSpeed;
}

bool UDSCharacterMovementComponent::IsMovingOnGround() const
{
	if(IsFlying())
	{
		return false; 
	}
	
	return Super::IsMovingOnGround();
}

bool UDSCharacterMovementComponent::CanLand() const
{
	if (false == IsFlying())
	{
		return false;
	}

	ACharacter* Character = GetCharacterOwner();
	if (false == IsValid(Character))
	{
		return false;
	}

	UCapsuleComponent* Capsule = Character->GetCapsuleComponent();
	if (false == IsValid(Capsule))
	{
		return false;
	}

	float HalfHeight = Capsule->GetScaledCapsuleHalfHeight();
	FVector Start = Character->GetActorLocation() - FVector(0, 0, HalfHeight * 0.5f);
	FVector End = Start - FVector(0, 0, GroundCheckThreshold);

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Character);

	DrawDebugLine(GetWorld(), Start, End, FColor::Blue, false, 0.1f, 0, 2.0f); 

	bool bHit = GetWorld()->LineTraceSingleByChannel(
		Hit,
		Start,
		End,
		ECC_Visibility,
		Params
	);

	return bHit;
}

void UDSCharacterMovementComponent::HandleMovementModeChanged(bool bCanFly, bool bIsCompleted)
{
	if (bCanFly)
	{
		ServerRPC_OnMovementModeChanged(EMovementMode::MOVE_Flying);
	}
	else
	{
		ServerRPC_OnMovementModeChanged(EMovementMode::MOVE_Walking);
	}
}

void UDSCharacterMovementComponent::ServerRPC_OnMovementModeChanged_Implementation(EMovementMode InMovementMode)
{
	SetMovementMode(InMovementMode);
}

void UDSCharacterMovementComponent::ServerMove_PerformMovement(const FCharacterNetworkMoveData& MoveData)
{
	Super::ServerMove_PerformMovement(MoveData);

	if (CharacterOwner && !CharacterOwner->bClientUpdating && CharacterOwner->IsPlayingRootMotion() && CharacterOwner->GetMesh())
	{
		CharacterOwner->GetMesh()->ConditionallyDispatchQueuedAnimEvents();
	}
}

FNetworkPredictionData_Client* UDSCharacterMovementComponent::GetPredictionData_Client() const
{
	if (ClientPredictionData == nullptr)
	{
		UDSCharacterMovementComponent* MutableThis = const_cast<UDSCharacterMovementComponent*>(this);
		MutableThis->ClientPredictionData = new FDSNetworkPredictionData_Client_Character(*this);
	}

	return ClientPredictionData;
}


void UDSCharacterMovementComponent::OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity)
{
	Super::OnMovementUpdated(DeltaSeconds, OldLocation, OldVelocity);
	if (bChangeSpeed)
	{
		bChangeSpeed = false;
	}

	// ApplyFloatingEffect(DeltaSeconds);
}

float UDSCharacterMovementComponent::GetMaxSpeed() const
{
	if (SpeedMode.Num() < 4 || FlightSpeedMode.Num() < 4)
	{
		return Super::GetMaxSpeed();
	}
	switch (MovementMode)
	{
	case MOVE_Walking:
	case MOVE_NavWalking:
		return IsCrouching() ? MaxWalkSpeedCrouched : SpeedMode[CurrentSpeedType];
	case MOVE_Falling:
		return SpeedMode[CurrentSpeedType];
	case MOVE_Swimming:
		return MaxSwimSpeed;
	case MOVE_Flying:
		return FlightSpeedMode[CurrentSpeedType];
	case MOVE_Custom:
		return MaxCustomMovementSpeed;
	case MOVE_None:
	default:
		return 0.f;
	}
}

void UDSCharacterMovementComponent::UpdateFromCompressedFlags(uint8 Flags)
{
	Super::UpdateFromCompressedFlags(Flags);

	uint8 ExtractedSpeed = Flags & 0xF0;
	ESpeedType SpeedType = static_cast<ESpeedType>(ExtractedSpeed);

	if(false == IsValid(CharacterOwner))
	{
		return;
	}

	if (CharacterOwner->GetLocalRole() == ROLE_Authority)
	{
		if (SpeedMode.Contains(SpeedType))
		{
			CurrentSpeedType = SpeedType;
		}
	}
}

void UDSCharacterMovementComponent::ApplyFloatingEffect(float DeltaSeconds)
{
	if (true == CanLand())
	{
		return;
	}

	ADSCharacter* Character = Cast<ADSCharacter>(GetCharacterOwner());
	
	if (false == IsValid(Character))
	{
		return;
	}
	UDSFlightComponent* FlightComponent = Character->GetFlightComponent();

	if (false == IsValid(FlightComponent))
	{
		return;
	}

	if (IsFlying())
	{
		float FloatStrength = 1.0f;
		float FloatSpeed = 4.0f;

		float Time = GetWorld()->GetTimeSeconds();
		float OffsetZ = FMath::Sin(Time * FloatSpeed) * FloatStrength;

		FVector NewLocation = Character->GetActorLocation();
		NewLocation.Z += OffsetZ;

		Character->SetActorLocation(NewLocation);
	}
}


FDSNetworkPredictionData_Client_Character::FDSNetworkPredictionData_Client_Character(const UCharacterMovementComponent& ClientMovement)
	:FNetworkPredictionData_Client_Character(ClientMovement)
{
}

FSavedMovePtr FDSNetworkPredictionData_Client_Character::AllocateNewMove()
{
	return FSavedMovePtr(new FDSSavedMove_Character());
}



void FDSSavedMove_Character::Clear()
{
	FSavedMove_Character::Clear();
	bChangeSpeed = false;
	
}

void FDSSavedMove_Character::SetInitialPosition(ACharacter* Character)
{
	FSavedMove_Character::SetInitialPosition(Character);

	UDSCharacterMovementComponent* DSMovement = Cast<UDSCharacterMovementComponent>(Character->GetCharacterMovement());

	if (true == IsValid(DSMovement))
	{
		bChangeSpeed = DSMovement->bChangeSpeed;
		SavedSpeedType = DSMovement->CurrentSpeedType;
	}
}

uint8 FDSSavedMove_Character::GetCompressedFlags() const
{
	uint8 Result = FSavedMove_Character::GetCompressedFlags();

	Result |= (static_cast<uint8>(SavedSpeedType) & 0xF0);

	return Result;
}
