#pragma once

// Default
#include "CoreMinimal.h"

// UE
#include "GameFramework/Actor.h"

// Game
#include "GameData/DSEnums.h"

// UHT
#include "DSWeapon.generated.h"

class UAnimMontage;

UCLASS()
class PROJECT25L_API ADSWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	ADSWeapon();
	
	UMeshComponent* GetMesh() const { return Mesh; }
	UStaticMeshComponent* GetStaticMesh() const { return Cast<UStaticMeshComponent>(Mesh); }
	USkeletalMeshComponent* GetSkeletalMesh() const { return Cast<USkeletalMeshComponent>(Mesh); }
	float GetInputThreshold() { return InputThreshold; }

	virtual void AttackPrimarySkill(const float& AutoAimAngle, const float& AttackDistance, const float& SkillDamage) const;
	virtual void EndPrimaryAttack() const;
	virtual void EndAttackSkill1();
	/* 클래스를 상속받았을 경우 반드시 구현해주어야하는 함수입니다. */
	virtual FVector GetFinalDestination(const float& AimAngle, const float& SkillDistance) const;

	virtual void PreAttackHitCheck(UAnimMontage* Anim);
	virtual void AttackHitCheck(ESkillType SkillTag, float SkillRange, float SkillDamage) {}
	virtual void PostAttackHitCheck() {}

protected:

	/*auto targeting*/
	//무기를 가진 모든 캐릭터는 오토 타겟팅이 가능하기 때문에 Weapon함수에 존재.
	FVector GetAutoTargetingLocation(const float& AimAngle, const FVector& StartLocation, const FVector& TargetLocation, const float& AttackRadius) const;

	virtual void InitializeData();
protected:

	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	
	UPROPERTY(Transient, Replicated)
	float InputThreshold;

	UPROPERTY(EditAnywhere, Category = "DSSettings | Weapon")
	EWeaponType WeaponType;
	
	UPROPERTY(EditAnywhere, Category = "DSSettings | Weapon")
	uint8 bUseAutoTargeting : 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DSSettings | Mesh", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UMeshComponent> Mesh;
	
	UPROPERTY(Transient)
	TObjectPtr<UAnimMontage> CurrentAttackAnim;
};
