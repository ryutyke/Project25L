// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Defualt
#include "CoreMinimal.h"

// UE
#include "Engine/DataTable.h"

// UHT
#include "DSSkillStat.generated.h"
/**
 * 
 */

 USTRUCT(BlueprintType)
 struct FDSPunchComboSkillStat : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	public:
	FDSPunchComboSkillStat() 
	: ComboSpeedRate(1.0f)
	, AttackRange(40.f)
	, AttackRadius (50.f)
	, AttackDamage(30.f)
	{}

	UPROPERTY(EditAnywhere, Category = PunchComboSkill)
	float ComboSpeedRate = 1.0f;
	UPROPERTY(EditAnywhere, Category = PunchComboSkill)
	float AttackRange = 40.f;
	UPROPERTY(EditAnywhere, Category = PunchComboSkill)
	float AttackRadius = 50.f;
	UPROPERTY(EditAnywhere, Category = PunchComboSkill)
	float AttackDamage = 30.f;


};