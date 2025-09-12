// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Default
#include "CoreMinimal.h"

// Game
#include "Skill/Base/DSSkillBase.h"

// UHT
#include "DSSkillBase_Girl.generated.h"


UCLASS(Blueprintable)
class PROJECT25L_API UDSSkillBase_Girl : public UDSSkillBase
{
	GENERATED_BODY()
	
public:
	UDSSkillBase_Girl(const FObjectInitializer& ObjectInitalize);
protected:
	virtual void OnSkillInitialized() override;

protected:
	UPROPERTY(Transient)
	float SkillDistance;

	UPROPERTY(Transient)
	float SkillWaitTime;
};
