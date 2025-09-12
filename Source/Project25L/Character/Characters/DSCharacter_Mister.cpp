// Default
#include "Character/Characters/DSCharacter_Mister.h"

// UE
#include "Kismet/KismetSystemLibrary.h"

// Game
#include "System/DSGameDataSubsystem.h"
#include "GameData/Skill/DSSkillAttribute.h"
#include "GameData/Physics/DSCollision.h"
#include "Components/DSStatComponent.h"
#include "Components/Input/DSPlayerInputComponent_Mister.h"
#include "Components/Skill/DSSkillControlComponent_Mister.h"
#include "DSLogChannels.h"

ADSCharacter_Mister::ADSCharacter_Mister(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SkillControlComponent = CreateDefaultSubobject<UDSSkillControlComponent_Mister>(TEXT("SkillControlComponent"));
	DSPlayerInputComponent = CreateDefaultSubobject<UDSPlayerInputComponent_Mister>(TEXT("DSPlayerInputComponent"));
}

void ADSCharacter_Mister::AttackHitCheck(const FName& HitSocket, const ESkillType& SkillTag)
{

	if (false == HasAuthority())
	{
		/*서버에서 히트 판정을 내린다.*/
		return;
	}

	UWorld* World = GetWorld();

	check(World);

	USkeletalMeshComponent* LocalMesh = GetMesh();

	if (false == IsValid(LocalMesh))
	{
		return;
	}

	UDSGameDataSubsystem* DataManager = UDSGameDataSubsystem::Get(this);
	check(DataManager);

	FDSSkillAttribute* SkillData = DataManager->GetDataRowByEnum<FDSSkillAttribute, ESkillType>(EDataTableType::MisterSkillAttributeData, SkillTag);

	if (nullptr == SkillData)
	{
		return;
	}

	FHitResult OutHitResult;
	const FVector& Location = LocalMesh->GetSocketLocation(HitSocket);
	TArray<AActor*> IgnoreActors;

	ETraceTypeQuery TraceType = UEngineTypes::ConvertToTraceType(CCHANNEL_DSACTION);

	IgnoreActors.Add(this);

	//HitSocket 위치로부터 cm 가량의 Overlap을 생성한다. => 1인 1타겟, 원은 시작점과 끝점이 같다.
	bool bIsHit = UKismetSystemLibrary::SphereTraceSingle(
		World,
		Location,
		Location,
		SkillData->SkillRadius,
		TraceType,
		false,
		IgnoreActors,
		EDrawDebugTrace::ForDuration,
		OutHitResult,
		true,
		FLinearColor::Red,
		FLinearColor::Green,
		1.f
	);

	if (bIsHit)
	{

		//TakeDamage
		ADSCharacterBase* TargetCharacter = Cast<ADSCharacterBase>(OutHitResult.GetActor());

		if (IsValid(TargetCharacter))
		{
			FDSDamageEvent DamageEvent;
			DamageEvent.DamageType = EDamageType::BaseAttack;
			
			TargetCharacter->TakeDamage(SkillData->Damage, DamageEvent, GetController(), this);
		}

	}
}