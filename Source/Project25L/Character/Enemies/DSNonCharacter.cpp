// Default
#include "Character/Enemies/DSNonCharacter.h"

// UE
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

// Game
#include "AI/DSAIControllerBase.h"
#include "Components/DSStatComponent.h"
#include "GameData/DSNonCharacterStat.h"
#include "UI/Game/Player/DSHPBar.h"
#include "System/DSEventSystems.h"

ADSNonCharacter::ADSNonCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AIControllerClass = ADSAIControllerBase::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;


	HealthComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
	HealthComponent->SetupAttachment(GetMesh(), TEXT("head"));
	HealthComponent->SetRelativeLocation(FVector(40.f, 0.f, 0.f));
}

void ADSNonCharacter::InitalizeData(int32 InNonCharacterID)
{
	NonCharacterID = InNonCharacterID;

	if (IsValid(StatComponent))
	{
		//위젯 컴포넌트를 가지고와서 Percentage를 반영해준다.
		StatComponent->InitializeStats(EDataTableType::NonCharacterData, ECharacterType::None, NonCharacterID);
	}
}

//float ADSNonCharacter::TakeFinalDamage(float DamageAmount, const FDSDamageEvent& NewDamageEvent, AController* EventInstigator, AActor* DamageCauser)
//{
//	float Result = Super::TakeFinalDamage(DamageAmount, NewDamageEvent, EventInstigator, DamageCauser);
//
//	if (IsValid(StatComponent))
//	{
//	}
//	
//	return Result;
//}

void ADSNonCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	UCharacterMovementComponent* LocalCharacterMovement = GetCharacterMovement();

	if (IsValid(LocalCharacterMovement))
	{
		FTimerHandle StunTimeHandler;
		UWorld* World = GetWorld();
		check(World);

		//현재 스탯 상태를 가져온다.
		//World->GetTimerManager().SetTimer(StunTimeHandler, 1.5f,);
		LocalCharacterMovement->SetMovementMode(EMovementMode::MOVE_Walking);
		/* 1.5초간 기절 상태 유지 */
	}
}

void ADSNonCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(HealthComponent))
	{
		HealthComponent->SetWidgetClass(HPWidgetClass.LoadSynchronous());
		HealthComponent->SetWidgetSpace(EWidgetSpace::Screen); //2D변경
		HealthComponent->SetDrawSize(FVector2D(60.f, 15.0f));
		HealthComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		UDSHPBar* HPBarWidget = Cast<UDSHPBar>(HealthComponent->GetWidget());

		if (IsValid(HPBarWidget))
		{
			/*초기화 => StatComponent가 이미 초기화 된 이후에 이 함수가 호출되어진다.*/
			DSEVENT_DELEGATE_BIND(StatComponent->OnHPChanged, HPBarWidget, &UDSHPBar::SetHP);
			//초기화 해준다.
			DSEVENT_DELEGATE_INVOKE(StatComponent->OnHPChanged, StatComponent->GetCurrentHP(), StatComponent->GetMaxHP());
		}
	}
}
