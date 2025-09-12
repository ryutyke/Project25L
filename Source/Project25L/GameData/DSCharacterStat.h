#pragma once
// Default
#include "CoreMinimal.h"

// UE
#include "Engine/DataTable.h"
#include "GameData/DSEnums.h"

// Game

// UHT
#include "DSCharacterStat.generated.h"

/**
 * @brief 캐릭터 스탯 구조체
 *
 * 이 구조체는 게임 캐릭터의 다양한 능력치를 정의합니다.
 * FTableRowBase를 상속받아 데이터 테이블 등에서 활용할 수 있습니다.
 */
USTRUCT(BlueprintType)
struct FDSCharacterStat : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	/**
	 * @brief 최대 체력 (기본 생명력)
	 *
	 * 캐릭터가 가질 수 있는 최대 체력 값을 나타냅니다.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Stats")
	float MaxHP;

	/**
	 * @brief 공격력
	 *
	 * 캐릭터의 공격력 수치를 나타냅니다.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Stats")
	float Attack;

	/**
	 * @brief 방어력 (인커밍 데미지 감소)
	 *
	 * 캐릭터가 받는 데미지를 감소시키는 방어력 값입니다.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Stats")
	float Defense;

	/**
	 * @brief 행운
	 *
	 * 치명타 확률 증가, 공격 피해량 증가, 받는 피해량 감소, 스킬 성능 향상 등에 영향을 주는 행운 수치입니다.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Stats")
	float Luck;

	/**
	 * @brief 이동 속도 배율
	 *
	 * 캐릭터의 이동 속도에 적용되는 배율 값입니다.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Stats")
	float MoveSpeed;

	/**
	 * @brief 초당 공격 횟수
	 *
	 * 캐릭터가 1초당 몇 번 공격할 수 있는지를 의미합니다.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Stats")
	float AttackSpeed;

	/**
	 * @brief 평타 공격 사거리 (m)
	 *
	 * 기본 공격의 사거리를 미터 단위로 나타냅니다.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Stats")
	float AttackRange;

	/**
	 * @brief 초당 체력 재생량 (HP)
	 *
	 * 캐릭터가 1초당 회복하는 체력의 양입니다.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Stats")
	float HPRegen;

	/**
	 * @brief 공격 시 데미지 대비 흡혈 비율 (%)
	 *
	 * 공격 시 입힌 데미지의 일정 비율만큼 체력을 회복하는 흡혈 비율입니다.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Stats")
	float HPSteal;

	/**
	 * @brief 주는 모든 데미지 증가 (%)
	 *
	 * 캐릭터가 주는 모든 데미지를 증가시키는 비율입니다.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Stats")
	float DamageBoost;

	/**
	 * @brief 받는 모든 데미지 감소 (%)
	 *
	 * 캐릭터가 받는 모든 데미지를 감소시키는 비율입니다.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Stats")
	float DamageReduction;

	/**
	 * @brief 스킬 쿨타임 감소 (sec)
	 *
	 * 스킬 사용 후 재사용 가능하기까지의 대기 시간을 감소시키는 값(초)입니다.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Stats")
	float CDReduction;

	/**
	 * @brief 치명타 발생 확률 (%)
	 *
	 * 공격 시 치명타가 발생할 확률을 퍼센트로 나타냅니다.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Stats")
	float CritChance;

	/**
	 * @brief 치명타 발생 시 데미지 증가율 (%)
	 *
	 * 치명타 발생 시 추가되는 데미지의 비율입니다.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Stats")
	float CritDamage;

	FDSCharacterStat()
		: MaxHP(0.f)
		, Attack(0.f)
		, Defense(0.f)
		, Luck(0.f)
		, MoveSpeed(0.f)
		, AttackSpeed(0.f)
		, AttackRange(0.f)
		, HPRegen(0.f)
		, HPSteal(0.f)
		, DamageBoost(0.f)
		, DamageReduction(0.f)
		, CDReduction(0.f)
		, CritChance(0.f)
		, CritDamage(0.f)
	{
	}
};