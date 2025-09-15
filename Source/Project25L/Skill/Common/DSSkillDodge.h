#pragma once

// Default
#include "CoreMinimal.h"

// Game
#include "Skill/Base/DSSkillBase.h"

// UHT
#include "DSSkillDodge.generated.h"

UCLASS()
class PROJECT25L_API UDSSkillDodge : public UDSSkillBase
{
	GENERATED_BODY()
	
public:
	UDSSkillDodge(const FObjectInitializer& ObjectInitalize);

protected:

	// 스킬의 핵심 기능을 실행하며, 파생 클래스에서 오버라이드하여 구체적인 로직을 구현하는 함수
	virtual void ActivateSkill(const FDSSkillSpecHandle Handle, const FDSSkillActorInfo* ActorInfo) override;

	// 스킬 실행 완료 후 종료를 알리고 후처리(자원 해제, 상태 초기화 등)를 수행 하는 함수
	virtual void EndSkill(const FDSSkillSpecHandle Handle, const FDSSkillActorInfo* ActorInfo, bool  bReplicateEndSkill, bool bWasCancelled) override;


};
