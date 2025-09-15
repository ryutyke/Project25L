//Defualt
#include "Skill/Base/DSSkillActorInfo.h"

//UE
#include "GameFramework/MovementComponent.h"

//Game
#include "Components/Skill/DSSkillControlComponent.h"
#include "DSLogChannels.h"


//*****************코드 리뷰(영민님 파~트) : IsValid, nullptr ****************************//
void FDSSkillActorInfo::SetCharacterActor(AActor* NewCharacterActor)
{
	InitFromActor(SkillOwner.Get(), NewCharacterActor, SkillControlComponent.Get());
}

void FDSSkillActorInfo::ClearActorInfo()
{
	SkillOwner = nullptr;
	SkillAvatar = nullptr;
	PlayerController = nullptr;
	SkeletalMeshComponent = nullptr;
	MovementComponent = nullptr;
}

void FDSSkillActorInfo::InitFromActor(AActor* InOwnerActor, AActor* InAvatarActor, UDSSkillControlComponent* InSkillControlComponent)
{
	check(InOwnerActor);
	check(InSkillControlComponent);

	SkillOwner = InOwnerActor;
	SkillAvatar = InAvatarActor;
	SkillControlComponent = InSkillControlComponent;

	AffectedAnimInstanceTag = InSkillControlComponent->AffectedAnimInstanceTag;

	APlayerController* OldPC = PlayerController.Get();

	// Look for a player controller or pawn in the owner chain.
	AActor* TestActor = InOwnerActor;
	while (IsValid(TestActor))
	{
		if (APlayerController* CastPC = Cast<APlayerController>(TestActor))
		{
			PlayerController = CastPC;
			break;
		}

		if (APawn* Pawn = Cast<APawn>(TestActor))
		{
			PlayerController = Cast<APlayerController>(Pawn->GetController());
			break;
		}

		TestActor = TestActor->GetOwner();
	}

	// Notify ASC if PlayerController was found for first time
	if (false == IsValid(OldPC) && PlayerController.IsValid())
	{
		InSkillControlComponent->OnPlayerControllerSet();
	}
	AActor* const AvatarActorPtr = SkillAvatar.Get();
	if (true == IsValid(AvatarActorPtr))
	{
		// Grab Components that we care about
		SkeletalMeshComponent = AvatarActorPtr->FindComponentByClass<USkeletalMeshComponent>();
		MovementComponent = Cast<UMovementComponent>(AvatarActorPtr->FindComponentByClass<UMovementComponent>());
	}
	else
	{
		SkeletalMeshComponent = nullptr;
		MovementComponent = nullptr;
	}
}

UAnimInstance* FDSSkillActorInfo::GetAnimInstance() const
{
	const USkeletalMeshComponent* SKMC = SkeletalMeshComponent.Get();

	if (IsValid(SKMC))
	{
		if (AffectedAnimInstanceTag != NAME_None)
		{
			if (UAnimInstance* Instance = SKMC->GetAnimInstance())
			{
				return Instance->GetLinkedAnimGraphInstanceByTag(AffectedAnimInstanceTag);
			}
		}

		return SKMC->GetAnimInstance();
	}

	return nullptr;
}

bool FDSSkillActorInfo::IsLocallyControlled() const
{
	if (const APlayerController* PC = PlayerController.Get())
	{
		return PC->IsLocalController();
	}
	else if (const APawn* OwnerPawn = Cast<APawn>(SkillOwner))
	{
		if (OwnerPawn->IsLocallyControlled())
		{
			return true;
		}
		else if (OwnerPawn->GetController())
		{
			// We're controlled, but we're not locally controlled.
			return false;
		}
	}

	return IsNetAuthority();
}

bool FDSSkillActorInfo::IsLocallyControlledPlayer() const
{
	if (const APlayerController* PC = PlayerController.Get())
	{
		return PC->IsLocalController();
	}

	return false;
}

bool FDSSkillActorInfo::IsNetAuthority() const
{
	// Make sure this works on pending kill actors
	AActor* const OwnerActorPtr = SkillOwner.Get(/*bEvenIfPendingKill=*/ true);
	if (OwnerActorPtr)
	{
		return (OwnerActorPtr->GetLocalRole() == ROLE_Authority);
	}

	return false;
}


