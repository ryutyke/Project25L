#pragma once

// Default
#include "CoreMinimal.h"

// Game
#include "Skill/Base/DSSkillBase_Mister.h"

// UHT
#include "DSPrimarySkill_Mister.generated.h"


UCLASS()
class PROJECT25L_API UDSPrimarySkill_Mister : public UDSSkillBase_Mister
{
	GENERATED_BODY()


public:
	UDSPrimarySkill_Mister(const FObjectInitializer& ObjectInitializer);

protected:
	virtual bool CanActivateSkill(const FDSSkillSpecHandle Handle, const FDSSkillActorInfo* ActorInfo);

	// 스킬의 핵심 기능을 실행하며, 파생 클래스에서 오버라이드하여 구체적인 로직을 구현하는 함수
	virtual void ActivateSkill(const FDSSkillSpecHandle Handle, const FDSSkillActorInfo* ActorInfo);
	
protected:

	UPROPERTY(Transient)
	TObjectPtr<UAnimMontage> AnimMontage;

	UPROPERTY(EditAnywhere, Category = "DSSettings | Montage")
	TSoftObjectPtr<UAnimMontage> AnimMontageClass;

	UPROPERTY(EditAnywhere, Category = "DSSettings | Montage")
	FString MontageSectionNamePrefix;


};
