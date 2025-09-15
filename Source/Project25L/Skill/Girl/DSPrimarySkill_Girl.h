#pragma once

// Default
#include "CoreMinimal.h"

// Game
#include "Skill/Base/DSSkillBase_Girl.h"

// UHT
#include "DSPrimarySkill_Girl.generated.h"


UCLASS(Blueprintable)
class PROJECT25L_API UDSPrimarySkill_Girl : public UDSSkillBase_Girl
{
	GENERATED_BODY()

public:
	UDSPrimarySkill_Girl(const FObjectInitializer& ObjectInitalize);

protected:
	// 스킬 활성화 전의 사전 처리(예: 상태 초기화, 로깅, 전처리 작업 등)를 수행한 후 실제 ActivateSkill()를 호출하는 함수
	virtual void CallActivateSkill(const FDSSkillSpecHandle Handle, const FDSSkillActorInfo* ActorInfo) override;

	// 스킬의 핵심 기능을 실행하며, 파생 클래스에서 오버라이드하여 구체적인 로직을 구현하는 함수
	virtual void ActivateSkill(const FDSSkillSpecHandle Handle, const FDSSkillActorInfo* ActorInfo) override;
	
	// 스킬 실행 완료 후 종료를 알리고 후처리(자원 해제, 상태 초기화 등)를 수행 하는 함수
	virtual void EndSkill(const FDSSkillSpecHandle Handle, const FDSSkillActorInfo* ActorInfo, bool  bReplicateEndSkill, bool bWasCancelled) override;


};
