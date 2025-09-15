#pragma once


#include "CoreMinimal.h"
// UE
#include "UObject/Interface.h"

// UHT
#include "DSSkillLifeCycle.generated.h"

UINTERFACE(MinimalAPI)
class UDSSkillLifeCycle : public UInterface
{
	GENERATED_BODY()
};

class UDSSkillControlComponent;
class UDSSkillBase;

/**
 * SkillSystem에서 가장 작은 단위인 Skill의 가장 작은 단위 클래스인 USkillBase의 실행 주기를 제어하는 인터페이스
 * USkillBase 객체의 생성, 실행, 종료 등 전체 생명 주기를 관리
 */
class PROJECT25L_API IDSSkillLifeCycle
{
	GENERATED_BODY()

public:
    /** 스킬을 소유한 Actor 반환 */
    virtual AActor* GetSkillOwner() const PURE_VIRTUAL(IDSSkillLifeCycle::GetSkillOwner, return nullptr;);

    /** 스킬을 실행하는 Actor 반환 */
    virtual AActor* GetSkillAvatar() const PURE_VIRTUAL(IDSSkillLifeCycle::GetSkillAvatar, return nullptr;);

    /** 스킬을 관리하는 SkillControlComponent 반환 */
    virtual UDSSkillControlComponent* GetSkillControlComponent() const PURE_VIRTUAL(IDSSkillLifeCycle::GetSkillControlComponent, return nullptr;);

    /** 스킬 초기화 시 호출 */
    virtual void OnSkillInitialized() {}

    /** 스킬 활성화 시 호출 */
    virtual void OnSkillActivated() {}

    /** 스킬 비활성화 시 호출 */
    virtual void OnSkillDeactivated() {}

};
