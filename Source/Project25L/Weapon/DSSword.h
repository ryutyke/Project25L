#pragma once

// Default
#include "CoreMinimal.h"

// Game
#include "Weapon/DSWeapon.h"

// UHT
#include "DSSword.generated.h"


UCLASS()
class PROJECT25L_API ADSSword : public ADSWeapon
{
	GENERATED_BODY()

public:
	ADSSword();

protected:
	virtual void BeginPlay() override;

	virtual void PrimaryAttackHitCheck(float SkillDamage);
	void PerformTraceWithDamage(const FVector& Start, const FVector& End, float SkillDamage);
	void StraightTraceWithDamage(const TArray<FVector>& PrevSocketPos, const TArray<FVector>& CurSocketPos, float SkillDamage);
	void ZigzagTraceWithDamage(const TArray<FVector>& PrevSocketPos, const TArray<FVector>& CurSocketPos, float SkillDamage);
	void DeltaZigzagTraceWithDamage(const TArray<FVector>& PrevSocketPos, const TArray<FVector>& CurSocketPos, float SkillDamage);
	void MetaZigzagTraceWithDamage(const TArray<FVector>& PrevSocketPos, const TArray<FVector>& CurSocketPos, float PrevTime, float CurTime, float SkillDamage, const FTransform& InPrevComponentToWorld, const FTransform& InCurComponentToWorld, double PrevLongestLengthSquared = std::numeric_limits<double>::max());
	
	// World Space
	void UpdateHitSocketLocations();

	bool GetSocketPosAtTime(float AnimTime, TArray<FVector>& OutSocketPos, const FTransform& InComponentToWorld);

	float GetCurrentAnimTime();

public:
	virtual void PreAttackHitCheck(UAnimMontage* Anim) override;
	virtual void AttackHitCheck(ESkillType SkillTag, float SkillRange, float SkillDamage) override;
	virtual void PostAttackHitCheck() override;

private:
	UPROPERTY(EditAnywhere, Category="Hit Trace")
	TArray<FName> HitSocketNames;

	int32 HitSocketCnt = 0;

	UPROPERTY(EditAnywhere, Category="Hit Trace")
	TEnumAsByte<ECollisionChannel> TraceChannel = ECC_Pawn;

	UPROPERTY(EditAnywhere, Category="Hit Trace")
	bool bDebugTrace = false;

	UPROPERTY(EditAnywhere, Category="Hit Trace")
	bool bDebugSphere = false;

	UPROPERTY(EditAnywhere, Category="Hit Trace")
	bool bStepTrace = false;

	UPROPERTY(EditAnywhere, Category="Hit Trace")
	bool bMetaTrace = false;

	TArray<FVector> PrevHitSocketLocations;
	TArray<FVector> HitSocketLocations;
	TArray<FVector> HitSocketStepDelta;
	TArray<FTransform> HitSocketToBoneTransforms;

	FTransform PrevComponentToWorld;
	FTransform CurComponentToWorld;

	float PrevAnimTime;
	float CurrentAnimTime;

	TSet<TWeakObjectPtr<AActor>> AlreadyHitActors;
	
	UPROPERTY(EditAnywhere, Category="Hit Trace")
	float InterpolationThreshold = 55.0f;

	FCollisionQueryParams QueryParams;

	FTransform SMeshToBone;
};
