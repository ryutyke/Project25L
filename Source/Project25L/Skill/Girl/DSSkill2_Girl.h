// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
// Default
#include "CoreMinimal.h"
#include "Skill/Base/DSSkillBase_Girl.h"

// Game
#include "GameData/DSEnums.h"

// UHT
#include "DSSkill2_Girl.generated.h"

UENUM(BlueprintType)
enum class EDSPortalState : uint8
{
	None,
	EntrancePlaced,	// 블랙홀 설치됨
	ExitPlaced,			// 화이트홀 설치됨
};

UENUM(BlueprintType)
enum class EDSPortalType : uint8
{
	None,
	Entrance,
	Exit
};

UENUM(BlueprintType)
enum class EDSPortalSurfaceType : uint8
{
	Floor,     // 바닥
	Wall,      // 벽
	Ceiling    // 천장
};


 class ADSGun;
 class UNiagaraComponent;
 class UNiagaraSystem;
 class ADSPortalActor;
 class ADSCharacter;

USTRUCT(BlueprintType)
struct FPortalInstallationParams 
{
	GENERATED_BODY()

	// 생성할 포탈 액터 클래스
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<ADSPortalActor> PortalClass;

	// 설치할 때 사용할 Offset (EndLocation에서 얼마나 뒤로 뺄지)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float InstallationOffset = 50.f;
};

UCLASS()
class PROJECT25L_API UDSSkill2_Girl : public UDSSkillBase_Girl
{
	GENERATED_BODY()

public:
	UDSSkill2_Girl(const FObjectInitializer& ObjectInitalize);

protected:
	// 스킬의 핵심 기능을 실행하며, 파생 클래스에서 오버라이드하여 구체적인 로직을 구현하는 함수
	virtual void ActivateSkill(const FDSSkillSpecHandle Handle, const FDSSkillActorInfo* ActorInfo) override;

	// 스킬 실행 완료 후 종료를 알리고 후처리(자원 해제, 상태 초기화 등)를 수행 하는 함수
	virtual void EndSkill(const FDSSkillSpecHandle Handle, const FDSSkillActorInfo* ActorInfo, bool  bReplicateEndSkill, bool bWasCancelled) override;

	virtual void InternalTick(float DeltaTimes) override;
	virtual void BeginPlay() override;
	UFUNCTION()
	void ActivatePortal();

protected:
	UPROPERTY(Transient)
	TObjectPtr<ADSGun> Gun;

	UPROPERTY()
	EDSPortalState CurrentPortalState;

	TMap<EDSPortalType, ADSPortalActor*> Portals;

	UPROPERTY(EditAnywhere, Category = "DSSetting|Portal")
	TMap<EDSPortalType, FPortalInstallationParams> PortalInstallationParams;

	FTimerHandle PortalActivateTimerHandle;
	float PortalElapsedTime = 0.f;

	FVector ExitLocation;
};
