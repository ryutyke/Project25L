
#pragma once

// Default
#include "CoreMinimal.h"

// Game
#include "Weapon/DSWeapon.h"
#include "Skill/Girl/DSSkill2_Girl.h"

// UHT
#include "DSGun.generated.h"

class UTimelineComponent;
class UNiagaraComponent;
class UNiagaraSystem;
class ADSPortalActor;
class ADSArmedCharacter;

UCLASS()
class PROJECT25L_API ADSGun : public ADSWeapon
{
	GENERATED_BODY()
	
public:
	ADSGun();

/*총과 관련된 함수 중 공통적으로 사용되어지는 함수 모음*/
public:
	//이 함수는 설치영역을 만드는 함수
	bool TryInstallInRange(FHitResult& HitResult, ECollisionChannel CollisionChannel, const float& AimAngle, const float& SkillDistance);

protected:
	//무기를 가졌는데, 원거리 공격을 수행할 경우 탄 퍼짐 현상이 존재한다. 이때 사용하는 함수
	FVector GetSpreadShotTargetLocation(const FVector& TargetDir) const;
	//총의 최종 목적지 계산
	virtual FVector GetFinalDestination(const float& AimAngle, const float& SkillDistance) const override;
	//총구 위치를 알기 위한 함수
	FVector GetMuzzleSocketLocation() const;
	//데이터 초기화 함수
	virtual void InitializeData() override;


/*Primary Skill 영역*/
protected:
	virtual void AttackPrimarySkill(const float& AutoAimAngle, const float& AttackDistance, const float& SkillDamage) const;

protected:
	UFUNCTION(Server, Reliable)
	void ServerRPC_PrimaryAttack(const FVector& StartLoc, const FVector& TargetDir, const float& SkillDamage) const;


/*Skill1 영역*/
public:	
	void AttackSkill1(const TSoftObjectPtr<UNiagaraSystem>& NiagaraSystem, const FVector& TargetLoc, const float& AttackRadius, const float& Impulse, const float& SkillDamage, const float& Duration);

protected:
	virtual void EndAttackSkill1() override;

protected:
	UFUNCTION(Server, Reliable)
	void ServerRPC_Skill1Attack(const TSoftObjectPtr<UNiagaraSystem>& NiagaraSystem, const FVector& TargetLoc, const float& SkillDistance, const float& Impulse, const float& SkillDamage, const float& Duration);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_Skill1Attack(const TSoftObjectPtr<UNiagaraSystem>& NiagaraSystem, const FVector& TargetLocation, const float& Duration);

/*Skill2 영역*/
public:
	ADSPortalActor* TryPlacePortalEffect(const FPortalInstallationParams &PortalInstallationParams, const float& AimAngle, const float& SkillDistance, FVector& ExitLocation);

protected:
	ADSPortalActor* InternalSpawnPortal(TSubclassOf<ADSPortalActor> PortalActorClass, EDSPortalSurfaceType SurfaceType, const FVector& TargetLocation, const FRotator& EffectRotation);
	bool CanPlacePortalAtLocation(const FVector& TargetLocation, ADSArmedCharacter* Character);
	void GetPortalSurfaceData(FVector &HitNormal, EDSPortalSurfaceType &PortalSurfaceType) const;
protected:
	UFUNCTION(Server, Reliable)
	void ServerRPC_Skill2Attack(TSubclassOf<ADSPortalActor> PortalActorClass, EDSPortalSurfaceType SurfaceType, const FVector& TargetLocation, const FRotator& TargetRotation);
	
protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;

protected:
	UPROPERTY(Transient)
	TArray<TWeakObjectPtr<AActor>> GravityGunTargets;

	UPROPERTY(Transient, Replicated)
	float SpreadCoef;

	UPROPERTY(Transient)
	TObjectPtr<UCurveFloat> YawRecoil;

	UPROPERTY(Transient)
	TObjectPtr<UCurveFloat> PitchRecoil;

	UPROPERTY(Transient)
	TArray<UNiagaraComponent*> PortalNiagaras;
	
	UPROPERTY(Transient)
	mutable int32 ShotsFired;

	UPROPERTY(EditAnywhere, Category = "DSSettings | Path")
	FString FilePath;
	
	UPROPERTY(EditAnywhere, Category = "DSSettings | Curve")
	TSoftObjectPtr<UCurveFloat> YawRecoilClass;

	UPROPERTY(EditAnywhere, Category = "DSSettings | Curve")
	TSoftObjectPtr<UCurveFloat> PitchRecoilClass;
};
