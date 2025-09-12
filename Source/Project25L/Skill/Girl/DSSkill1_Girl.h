#pragma once

// Default
#include "CoreMinimal.h"

// Game
#include "Skill/Base/DSSkillBase_Girl.h"

// UHT
#include "DSSkill1_Girl.generated.h"

UENUM()
enum class EDSGravityStatus : uint8
{
	None,		//초기
	Installed,	//설치
	Confirm,	//확정
	Cancel,		//취소
	Activate,	//발동
};

class UNiagaraComponent;
class ADSGun;
class ADSGridPlane;

UCLASS()
class PROJECT25L_API UDSSkill1_Girl : public UDSSkillBase_Girl
{
	GENERATED_BODY()
	
public:
	UDSSkill1_Girl(const FObjectInitializer& ObjectInitalize);
	
	void SpawnGravity();
	void CancelGravity();
	void ConfirmGravity();
protected:
	// 스킬 활성화 가능 여부(자원, 쿨타임, 상태 조건 등)를 확인하는 함수
	virtual bool CanActivateSkill(const FDSSkillSpecHandle Handle, const FDSSkillActorInfo* ActorInfo);

	virtual void InternalTick(float DeltaTime) override;

	// 스킬의 핵심 기능을 실행하며, 파생 클래스에서 오버라이드하여 구체적인 로직을 구현하는 함수
	virtual void ActivateSkill(const FDSSkillSpecHandle Handle, const FDSSkillActorInfo* ActorInfo);

	// 스킬 실행 완료 후 종료를 알리고 후처리(자원 해제, 상태 초기화 등)를 수행 하는 함수
	virtual void EndSkill(const FDSSkillSpecHandle Handle, const FDSSkillActorInfo* ActorInfo, bool  bReplicateEndSkill, bool bWasCancelled);

protected:


	UPROPERTY(Transient)
	FIntPoint PreGrid;
	
	UPROPERTY(Transient)
	FVector PreTargetLocation;

	UPROPERTY(Transient)
	TWeakObjectPtr<ADSGun> Weapon;
	
	UPROPERTY(Transient)
	TWeakObjectPtr<ADSGridPlane> GridPlane;

	UPROPERTY(Transient)
	TObjectPtr<UNiagaraComponent> NiagaraEffect;

	UPROPERTY(Transient)
	uint8 bIsInstalled;

	UPROPERTY(Transient)
	EDSGravityStatus GravityStatus;

	UPROPERTY(Transient)
	FTimerHandle CancelSkillTimer;
	
	UPROPERTY(EditAnywhere, Category = "DSSettings | Skill1")
	float GravityImpulse;
};
