// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Default
#include "CoreMinimal.h"

// Game
#include "Skill/Base/DSSkillBase.h"

// UHT
#include "DSSkillBase_Mister.generated.h"

UCLASS()
class PROJECT25L_API UDSSkillBase_Mister : public UDSSkillBase
{
GENERATED_BODY()

public:
	UDSSkillBase_Mister(const FObjectInitializer& ObjectInitalize);
protected:
	virtual void OnSkillInitialized() override;
};
