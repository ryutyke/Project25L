// Fill out your copyright notice in the Description page of Project Settings.


#include "Skill/Base/DSTestSkill.h"

UDSTestSkill::UDSTestSkill()
:Super()
{
	CooldownPolicy = ESkillCooldownPolicy::CooldownAfterActive;
	bSkillHasDuration = true;
}

UDSTestSkill::UDSTestSkill(FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	bSkillHasDuration = true;
}



// 
UDSTestSkill_1::UDSTestSkill_1()
{

	InstancingPolicy = ESkillInstancingPolicy::NonInstanced;
}

UDSTestSkill_1::UDSTestSkill_1(FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

	InstancingPolicy = ESkillInstancingPolicy::NonInstanced;
}

UDSTestSkill_2::UDSTestSkill_2()
{
}

UDSTestSkill_2::UDSTestSkill_2(FObjectInitializer& ObjectInitializer)
{
}
