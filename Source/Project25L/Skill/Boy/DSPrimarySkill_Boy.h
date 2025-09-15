#pragma once

// Default
#include "CoreMinimal.h"

// Game
#include "Skill/Boy/DSSkillBase_Boy.h"

// UHT
#include "DSPrimarySkill_Boy.generated.h"


UCLASS()
class PROJECT25L_API UDSPrimarySkill_Boy : public UDSSkillBase_Boy
{
	GENERATED_BODY()

public:
	UDSPrimarySkill_Boy(const FObjectInitializer& ObjectInitalize);

protected:
	virtual void ActivateSkill(const FDSSkillSpecHandle Handle, const FDSSkillActorInfo* ActorInfo) override;
};
