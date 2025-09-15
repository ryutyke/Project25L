// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Skill/Base/DSSkillBase.h"
#include "DSTestSkill.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT25L_API UDSTestSkill : public UDSSkillBase
{
	GENERATED_BODY()

public:
	UDSTestSkill();
	UDSTestSkill (FObjectInitializer &ObjectInitializer);
};


UCLASS()
class PROJECT25L_API UDSTestSkill_1 : public UDSSkillBase
{
	GENERATED_BODY()

public:
	UDSTestSkill_1();
	UDSTestSkill_1(FObjectInitializer& ObjectInitializer);

};

UCLASS()
class PROJECT25L_API UDSTestSkill_2 : public UDSSkillBase
{
	GENERATED_BODY()

public:
	UDSTestSkill_2();
	UDSTestSkill_2(FObjectInitializer& ObjectInitializer);
};