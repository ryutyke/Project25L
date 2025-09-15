#pragma once

// Default
#include "CoreMinimal.h"

// UE
#include "Engine/DataTable.h"

// Game
#include "GameData/DSEnums.h"

// UHT
#include "DSSkillAttribute.generated.h"

/**
 *  스킬의 수치 데이터를 관리하는 DataTable
 */
class UNiagaraSystem;

USTRUCT(BlueprintType)
struct  FDSSkillAttribute : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	FDSSkillAttribute() :
		Damage(0.f)
		, MaxCooltime(0.0f)
		, AutoAimAngle(0.0f)
		, SkillRadius(0.f)
		, InputThreshold(0.f)
		, SkillDuration(0.f)
		, Effects()
	{
	}
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DSSettings | Skill")
	float Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DSSettings | Skill")
	float MaxCooltime;

	/*오토 타겟팅용 각도를 의미*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DSSettings | Skill")
	float AutoAimAngle;

	/* 스킬 범위 => SkillRadius */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DSSettings | Skill")
	float SkillRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DSSettings | Skill")
	float InputThreshold;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DSSettings | Skill")
	float SkillDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DSSettings | Skill")
	float MinDodgeSpeed;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DSSettings | Skill")
	float MaxDodgeSpeed;

	/*스킬 발동시 보여줄 이펙트*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DSSettings | Skill")
	TMap<ESkillActivationStatus, TSoftObjectPtr<UNiagaraSystem>> Effects;
};

USTRUCT(BlueprintType)
struct  FDSSkillAttribute_Girl : public FDSSkillAttribute
{
	GENERATED_USTRUCT_BODY()

public:
	FDSSkillAttribute_Girl() :
		SKillDistance(0.0f)
		, SkillWaitTime(0.0f)
	{
	}

	/* 총구에서부터 Linetrace를 쏘는 거리 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Category = "DSSettings | Girls")
	float SKillDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DSSettings | Girls")
	float SkillWaitTime;
};

USTRUCT(BlueprintType)
struct  FDSSkillAttribute_Boy : public FDSSkillAttribute
{
	GENERATED_USTRUCT_BODY()

public:
	FDSSkillAttribute_Boy()
	{
	}
};