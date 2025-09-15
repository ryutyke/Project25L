//Default
#include "Character/DSCharacterBase.h"

//UE
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

//Game
#include "Components/DSStatComponent.h"
#include "Components/Skill/DSSkillControlComponent.h"
#include "DSLogChannels.h"
#include "System/DSEventSystems.h"
#include "System/DSGameDataSubsystem.h"
#include "Types/DSDamageEvents.h"

ADSCharacterBase::ADSCharacterBase(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	//Pawn 
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	GetCharacterMovement()->MaxWalkSpeed = 450.f;
	GetCharacterMovement()->JumpZVelocity = 0.f; //점프 할 수 없음.
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.0f, 0.f); //FORCEINLINE FRotator(T InPitch, T InYaw, T InRoll);
	GetCharacterMovement()->AirControl = 0.35f; //점프가 없기때문에 공중에 날 확률은 적지만, 있을 수도 있음.
	GetCharacterMovement()->bOrientRotationToMovement = false; //캐릭터나 액터가 이동할 때 그 방향으로 회전을 수행(마우스에 따라서 회전을 수행하도록 도와줌)
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f; //점차 멈추는 효과 
	GetCharacterMovement()->bWantsToCrouch = false;
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	//CapsuleComponent
	GetCapsuleComponent()->InitCapsuleSize(25.f, 90.0f);

	//SkeletalMesh
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -90.f), FRotator(0.0f, -90.f, 0.f));
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	GetMesh()->SetCollisionProfileName(TEXT("NoCollision"));

	/*아래 코드 삭제 예정*/
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SkeletalMeshRef(TEXT("/Script/Engine.SkeletalMesh'/Game/Characters/Mannequins/Meshes/SKM_Manny.SKM_Manny'"));

	if (SkeletalMeshRef.Object)
	{
		GetMesh()->SetSkeletalMesh(SkeletalMeshRef.Object);
	}

	StatComponent = CreateDefaultSubobject<UDSStatComponent>(TEXT("StatComponent"));
}

UAnimInstance* ADSCharacterBase::GetAnimInstance()
{
	USkeletalMeshComponent* SkeletalMeshComponent = GetMesh();

	if (IsValid(SkeletalMeshComponent))
	{
		return SkeletalMeshComponent->GetAnimInstance();
	}

	return nullptr;
}

float ADSCharacterBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	UDSStatComponent* StatComponentPtr = GetStatComponent();
	if (nullptr == StatComponentPtr)
	{
		return 0.f;
	}

	if (DamageEvent.GetTypeID() == FDSDamageEvent::ClassID)
	{
		const FDSDamageEvent* DSDamageEvent = static_cast<const FDSDamageEvent*>(&DamageEvent);
		StatComponentPtr->ReceiveDamage(DamageAmount, DSDamageEvent->DamageType, DSDamageEvent->ElementType, this);

		return DamageAmount;
	}

	return 0.f;
}

void ADSCharacterBase::BeginPlay()
{
	Super::BeginPlay();
}

void ADSCharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	InitSkillActorInfo();
}

void ADSCharacterBase::OnRep_Controller()
{
	Super::OnRep_Controller();
	
	InitSkillActorInfo();
}

void ADSCharacterBase::InitSkillActorInfo()
{
	if (GetSkillControlComponent())
	{
		GetSkillControlComponent()->InitSkillActorInfo(this, this);
		GetSkillControlComponent()->InitializeSkillSpecs();
	}
}

