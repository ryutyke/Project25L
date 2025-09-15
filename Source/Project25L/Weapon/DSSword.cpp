// Default
#include "Weapon/DSSword.h"

// UE
#include "Kismet/GameplayStatics.h"

// Game
#include "interface/DamageableInterface.h"
#include "Types/DSDamageEvents.h"
#include "GameData/Physics/DSCollision.h"
#include "GameData/Animation/BoneToComponentAnimMetaData.h"
#include "Character/DSArmedCharacter.h"
#include "DSLogChannels.h"

ADSSword::ADSSword()
	: Super()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject< UStaticMeshComponent>(TEXT("Mesh"));
}

void ADSSword::BeginPlay()
{
	AActor* OwnerActor = GetOwner();
	if (false == IsValid(OwnerActor))
	{
		return;
	}

	QueryParams.AddIgnoredActor(OwnerActor);
	QueryParams.AddIgnoredActor(this);

	if (nullptr == Mesh)
	{
		return;
	}

	PrevHitSocketLocations.Empty();
	HitSocketLocations.Empty();
	AlreadyHitActors.Empty();

	HitSocketCnt = HitSocketNames.Num();

	ADSArmedCharacter* OwnerCharacter = Cast<ADSArmedCharacter>(OwnerActor);
	if (false == IsValid(OwnerCharacter))
	{
		return;
	}

	USkeletalMeshComponent* MeshComp = OwnerCharacter->GetMesh();
	if (nullptr == MeshComp)
	{
		return;
	}

	FName SocketName = OwnerCharacter->GetWeaponSocketName(EWeaponSocketType::Equipped);
	const FTransform& MeshToSword = Mesh->GetRelativeTransform(); // 6번
	const FTransform& SwordToBoneSocket = OwnerCharacter->GetWeaponRelativeTransform(); // 5번
	const FTransform& BoneSocketToBone = MeshComp->GetSocketTransform(SocketName, RTS_ParentBoneSpace); // 4번
	SMeshToBone = MeshToSword * SwordToBoneSocket * BoneSocketToBone;

	for (int32 SocketIndex = 0; SocketIndex < HitSocketCnt; ++SocketIndex)
	{
		const FName HitSocketName = HitSocketNames[SocketIndex];
		const FTransform& SocketRelativeTransform = Mesh->GetSocketTransform(HitSocketName, RTS_Actor);
		
		HitSocketToBoneTransforms.Add(SocketRelativeTransform * SMeshToBone); // 7번
	}
}

void ADSSword::PrimaryAttackHitCheck(float SkillDamage)
{
	UpdateHitSocketLocations();
	CurrentAnimTime = GetCurrentAnimTime();
	if (CurrentAnimTime < 0)
	{
		return;
	}

	if (bMetaTrace)
	{
		AActor* OwnerActor = GetOwner();
		if (false == IsValid(OwnerActor))
		{
			return;
		}

		ADSArmedCharacter* OwnerCharacter = Cast<ADSArmedCharacter>(OwnerActor);
		if (false == IsValid(OwnerCharacter))
		{
			return;
		}

		USkeletalMeshComponent* MeshComp = OwnerCharacter->GetMesh();
		if (nullptr == MeshComp)
		{
			return;
		}

		CurComponentToWorld = MeshComp->GetComponentToWorld();

		MetaZigzagTraceWithDamage(PrevHitSocketLocations, HitSocketLocations, PrevAnimTime, CurrentAnimTime, SkillDamage, PrevComponentToWorld, CurComponentToWorld);
	}

	else if (bStepTrace)
	{
		DeltaZigzagTraceWithDamage(PrevHitSocketLocations, HitSocketLocations, SkillDamage);
	}

	else
	{
		ZigzagTraceWithDamage(PrevHitSocketLocations, HitSocketLocations, SkillDamage);
	}
	
	PrevHitSocketLocations = HitSocketLocations;
	PrevAnimTime = CurrentAnimTime;
	PrevComponentToWorld = CurComponentToWorld;
}

void ADSSword::PerformTraceWithDamage(const FVector& Start, const FVector& End, float SkillDamage)
{
	UWorld* World = GetWorld();
	check(World);

	TArray<FHitResult> HitResults;
	bool bHit = World->LineTraceMultiByChannel(
		HitResults,
		Start,
		End,
		TraceChannel,
		QueryParams
	);

	if (bDebugTrace)
	{
		const FColor DebugColor = bHit ? FColor::Red : FColor::Green;
		DrawDebugLine(World, Start, End, DebugColor, false, 1.5f, 0, 1.0f);
	}

	if (bHit)
	{
		FDSDamageEvent DamageEvent;
		DamageEvent.DamageType = EDamageType::BaseAttack;

		for (const FHitResult& Hit : HitResults)
		{
			AActor* TargetActor = Hit.GetActor();

			if (IsValid(TargetActor) && !AlreadyHitActors.Contains(TargetActor))
			{
				AlreadyHitActors.Add(TargetActor);

				TargetActor->TakeDamage(SkillDamage, DamageEvent, GetOwner()->GetInstigatorController(), this);
			}
		}
	}
}

void ADSSword::StraightTraceWithDamage(const TArray<FVector>& PrevSocketPos, const TArray<FVector>& CurSocketPos, float SkillDamage)
{
	for (int32 SocketIndex = 0; SocketIndex < HitSocketCnt; ++SocketIndex)
	{
		PerformTraceWithDamage(PrevSocketPos[SocketIndex], CurSocketPos[SocketIndex], SkillDamage);
	}
}

void ADSSword::ZigzagTraceWithDamage(const TArray<FVector>& PrevSocketPos, const TArray<FVector>& CurSocketPos, float SkillDamage)
{
	int LastSocketIndex = HitSocketCnt - 1;
	PerformTraceWithDamage(CurSocketPos[0], CurSocketPos[LastSocketIndex], SkillDamage);

	for (int32 SocketIndex = 0; SocketIndex < HitSocketCnt; ++SocketIndex)
	{
		PerformTraceWithDamage(PrevSocketPos[SocketIndex], CurSocketPos[SocketIndex], SkillDamage);

		int32 NextSocketIndex = SocketIndex + 1;
		if (NextSocketIndex < HitSocketCnt)
		{
			PerformTraceWithDamage(PrevSocketPos[SocketIndex], CurSocketPos[NextSocketIndex], SkillDamage);
		}
	}
}

void ADSSword::DeltaZigzagTraceWithDamage(const TArray<FVector>& PrevSocketPos, const TArray<FVector>& CurSocketPos, float SkillDamage)
{
	int32 EndSocketIndex = HitSocketCnt - 1;

	double LongestLengthSquared = FVector::DistSquared(PrevSocketPos[EndSocketIndex], CurSocketPos[EndSocketIndex]);
	
	if (InterpolationThreshold <= 0)
	{
		DS_LOG(DSSkillLog, Warning, TEXT("[DSSword] : InterpolationThreshold have to be bigger than 0"));
		
		return;
	}

	int32 StepCount = FMath::CeilToInt(LongestLengthSquared / FMath::Square(InterpolationThreshold));
	
	if (StepCount <= 1)
	{
		ZigzagTraceWithDamage(PrevSocketPos, CurSocketPos, SkillDamage);
		return;
	}

	HitSocketStepDelta.SetNum(HitSocketCnt);
	for (int32 SocketIndex = 0; SocketIndex < HitSocketCnt; ++SocketIndex)
	{
		FVector DeltaVector = CurSocketPos[SocketIndex] - PrevSocketPos[SocketIndex];
		HitSocketStepDelta[SocketIndex] = DeltaVector / StepCount;
	}

	int32 LastSocketIndex = HitSocketCnt - 1;

	for (int32 StepIndex = 0; StepIndex < StepCount; ++StepIndex)
	{
		// 칼날 시작부터 칼끝
		FVector FirstHitSocketLocation = PrevSocketPos[0] + HitSocketStepDelta[0] * (StepIndex + 1);
		FVector LastHitSocketLocation = PrevSocketPos[LastSocketIndex] + HitSocketStepDelta[LastSocketIndex] * (StepIndex + 1);
		PerformTraceWithDamage(FirstHitSocketLocation, LastHitSocketLocation, SkillDamage);

		for (int32 SocketIndex = 0; SocketIndex < HitSocketCnt; ++SocketIndex)
		{
			// 직선
			FVector PrevHitSocketLocation = PrevSocketPos[SocketIndex] + HitSocketStepDelta[SocketIndex] * StepIndex;
			FVector HitSocketLocation = PrevSocketPos[SocketIndex] + HitSocketStepDelta[SocketIndex] * (StepIndex + 1);

			PerformTraceWithDamage(PrevHitSocketLocation, HitSocketLocation, SkillDamage);

			// 대각선
			int32 NextSocketIndex = SocketIndex + 1;
			if (NextSocketIndex < HitSocketCnt)
			{
				FVector NextHitSocketLocation = PrevSocketPos[NextSocketIndex] + HitSocketStepDelta[NextSocketIndex] * (StepIndex + 1);
				PerformTraceWithDamage(PrevHitSocketLocation, NextHitSocketLocation, SkillDamage);
			}
		}
	}
}

void ADSSword::MetaZigzagTraceWithDamage(const TArray<FVector>& PrevSocketPos, const TArray<FVector>& CurSocketPos, float PrevTime, float CurTime, float SkillDamage, const FTransform& InPrevComponentToWorld, const FTransform& InCurComponentToWorld, double PrevLongestLengthSquared)
{
	if (PrevSocketPos.Num() != HitSocketCnt)
	{
		DS_LOG(DSSkillLog, Warning, TEXT("[DSSword] : Meta Hit Check Failed"));

		return;
	}

	int32 EndSocketIndex = HitSocketCnt - 1;
	
	// 맨 끝 소켓이 가장 멀다고 가정
	double LongestLengthSquared = FVector::DistSquared(CurSocketPos[EndSocketIndex], PrevSocketPos[EndSocketIndex]);
	
	// 무한 루프 방지
	if (LongestLengthSquared >= PrevLongestLengthSquared)
	{
		return;
	}
	
	bool bShouldDivide = FMath::Square(InterpolationThreshold) < LongestLengthSquared;

	if (bShouldDivide)
	{
		float MidTime = (PrevTime + CurTime) / 2;
		TArray<FVector> MidSocketPos;
		MidSocketPos.SetNumUninitialized(HitSocketCnt);

		FTransform MidComponentToWorld;
		MidComponentToWorld.Blend(InPrevComponentToWorld, InCurComponentToWorld, 0.5f); // 1번, 2번

		bool bResult = GetSocketPosAtTime(MidTime, MidSocketPos, MidComponentToWorld);

		if (bResult)
		{
			MetaZigzagTraceWithDamage(PrevSocketPos, MidSocketPos, PrevTime, MidTime, SkillDamage, PrevComponentToWorld, MidComponentToWorld, LongestLengthSquared);
			MetaZigzagTraceWithDamage(MidSocketPos, CurSocketPos, MidTime, CurTime, SkillDamage, MidComponentToWorld, CurComponentToWorld, LongestLengthSquared);
		}

		else
		{
			ZigzagTraceWithDamage(PrevSocketPos, CurSocketPos, SkillDamage);
		}
	}

	else
	{
		ZigzagTraceWithDamage(PrevSocketPos, CurSocketPos, SkillDamage);
	}
}

void ADSSword::UpdateHitSocketLocations()
{
	if (nullptr == Mesh)
	{
		return;
	}

	HitSocketLocations.SetNum(HitSocketCnt);

	for (int32 SocketIndex = 0; SocketIndex < HitSocketCnt; ++SocketIndex)
	{
		const FName SocketName = HitSocketNames[SocketIndex];
		const FVector CurrentLocation = Mesh->GetSocketLocation(SocketName);
		HitSocketLocations[SocketIndex] = CurrentLocation;
	}
}

bool ADSSword::GetSocketPosAtTime(float AnimTime, TArray<FVector>& OutSocketPos, const FTransform& InComponentToWorld)
{
	AActor* OwnerActor = GetOwner();
	if (false == IsValid(OwnerActor))
	{
		return false;
	}

	ADSArmedCharacter* OwnerCharacter = Cast<ADSArmedCharacter>(OwnerActor);
	if (false == IsValid(OwnerCharacter))
	{
		return false;
	}

	USkeletalMeshComponent* MeshComp = OwnerCharacter->GetMesh();
	if (nullptr == MeshComp)
	{
		return false;
	}

	if (nullptr == CurrentAttackAnim) 
	{
		return false;
	}

	// Montage 속 AnimSequence의 메타 데이터 사용할 경우
	/*
	UAnimSequenceBase* CurrentSequence = nullptr;
	float AnimSequenceTime; 
	
	for (const FSlotAnimationTrack& SlotTrack : CurrentAttackAnim->SlotAnimTracks)
	{
		for (const FAnimSegment& Segment : SlotTrack.AnimTrack.AnimSegments)
		{
			if (AnimTime >= Segment.StartPos && AnimTime <= Segment.GetEndPos())
			{
				CurrentSequence = Segment.GetAnimReference();
				AnimSequenceTime = Segment.AnimStartTime + ((AnimTime - Segment.StartPos) * Segment.AnimPlayRate);
			}
		}
	}

	if (nullptr == CurrentSequence)
	{
		return;
	}

	UBoneToComponentAnimMetaData* MetaData = nullptr;
	MetaData = CurrentSequence->FindMetaDataByClass<UBoneToComponentAnimMetaData>();
	*/

	UBoneToComponentAnimMetaData* MetaData = nullptr;
	MetaData = CurrentAttackAnim->FindMetaDataByClass<UBoneToComponentAnimMetaData>();
	
	if (nullptr == MetaData)
	{
		check(false);
		return false;
	}

	TArray<float>& KeyTimes = MetaData->Times;

	int32 CurrentIndex = 0;

	int32 Low = 0, High = KeyTimes.Num() - 1;
	while (Low + 1 < High)
	{
		int32 Mid = (Low + High) / 2;
		(KeyTimes[Mid] <= AnimTime) ? Low = Mid : High = Mid;
	}
	CurrentIndex = Low;

	
	/*
	Todo : (다른 곳에 적용하게 되면)
	1. Sampling Rate 변수로 넣어서 고려해 줘야 함. 
	2. a DeltaTime 만큼 빼줘야 하는 원인 찾기.
	*/
	int32 EmpiricalNum = FMath::Max(0, CurrentIndex - (11 * CurrentAttackAnim->RateScale));
	
	const FTransform& BoneToSKMComponent = MetaData->BoneToComponent[EmpiricalNum]; // 3번

	const FTransform& BoneToWorld = BoneToSKMComponent * InComponentToWorld; // 1번, 2번
	
	for (int32 SocketIndex = 0; SocketIndex < HitSocketCnt; ++SocketIndex)
	{
		const FTransform& HitSocketToWorld = HitSocketToBoneTransforms[SocketIndex] * BoneToWorld;

		OutSocketPos[SocketIndex] = HitSocketToWorld.GetLocation();

		if (bDebugSphere)
		{
			DrawDebugSphere(MeshComp->GetWorld(), OutSocketPos[SocketIndex], 2.5f, 1, FColor::Blue, false, 1.f);
		}
	}

	return true;
}

float ADSSword::GetCurrentAnimTime()
{
	AActor* OwnerActor = GetOwner();
	if (false == IsValid(OwnerActor))
	{
		return -1.f;
	}

	ACharacter* OwnerCharacter = Cast<ACharacter>(OwnerActor);
	if (false == IsValid(OwnerCharacter))
	{
		return -1.f;
	}

	USkeletalMeshComponent* MeshComp = OwnerCharacter->GetMesh();
	if (nullptr == MeshComp)
	{
		return -1.f;
	}

	UAnimInstance* AnimInstance = MeshComp->GetAnimInstance();
	if (nullptr == AnimInstance)
	{
		return -1.f;
	}

	if (false == ensure(nullptr != CurrentAttackAnim))
	{
		return -1.f;
	}

	FAnimMontageInstance* MontageInstance = AnimInstance->GetActiveInstanceForMontage(CurrentAttackAnim);
	if (nullptr == MontageInstance)
	{
		return -1.f;
	}

	float CurrentPosition = MontageInstance->GetPosition();
	
	return CurrentPosition;
}

void ADSSword::PreAttackHitCheck(UAnimMontage* Anim)
{
	Super::PreAttackHitCheck(Anim);

	if (nullptr == Mesh)
	{
		return;
	}

	PrevHitSocketLocations.Empty();
	HitSocketLocations.Empty();
	AlreadyHitActors.Empty();

	for (int32 SocketIndex = 0; SocketIndex < HitSocketCnt; ++SocketIndex)
	{
		const FName SocketName = HitSocketNames[SocketIndex];
		const FVector SocketWorldLocation = Mesh->GetSocketLocation(SocketName);
		PrevHitSocketLocations.Add(SocketWorldLocation);
	}

	PrevAnimTime = GetCurrentAnimTime();

	AActor* OwnerActor = GetOwner();
	if (false == IsValid(OwnerActor))
	{
		return;
	}

	ADSArmedCharacter* OwnerCharacter = Cast<ADSArmedCharacter>(OwnerActor);
	if (false == IsValid(OwnerCharacter))
	{
		return;
	}

	USkeletalMeshComponent* MeshComp = OwnerCharacter->GetMesh();
	if (nullptr == MeshComp)
	{
		return;
	}

	PrevComponentToWorld = MeshComp->GetComponentToWorld();
}

void ADSSword::AttackHitCheck(ESkillType SkillTag, float SkillRange, float SkillDamage)
{
	Super::AttackHitCheck(SkillTag, SkillRange, SkillDamage);

	switch (SkillTag)
	{
		case ESkillType::PrimarySkill:
			PrimaryAttackHitCheck(SkillDamage);
			break;
	}
}

void ADSSword::PostAttackHitCheck()
{
	Super::PostAttackHitCheck();

	PrevHitSocketLocations.Empty();
	HitSocketLocations.Empty();
	AlreadyHitActors.Empty();
	PrevAnimTime = 0.0f;
	CurrentAnimTime = 0.0f;
}


