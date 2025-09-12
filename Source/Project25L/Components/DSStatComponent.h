#pragma once
// Default
#include "CoreMinimal.h"

// UE
#include "Components/PawnComponent.h"
#include "Engine/DamageEvents.h"
#include "Net/Serialization/FastArraySerializer.h"

// Game
#include "GameData/DSCharacterStat.h"
#include "GameData/DSEnums.h"
#include "System/DSEventSystems.h"
#include "Interface/HealableInterface.h"
#include "Types/DSDamageEvents.h"


// UHT
#include "DSStatComponent.generated.h"

/** 게임 내 버프 효과 정보를 저장하는 구조체. */
USTRUCT(BlueprintType)
struct FBuffEntry : public FFastArraySerializerItem
{
	GENERATED_USTRUCT_BODY()

	FBuffEntry() = default;

	FBuffEntry(EDSStatType InStatType, EOperationType InOperationType, int32 InBuffID, float InBuffValue)
		: StatType(InStatType), OperationType(InOperationType), BuffID(InBuffID), BuffValue(InBuffValue)
	{
	}

	/** 버프가 적용될 스탯 유형. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DSSettings | Buff")
	EDSStatType StatType;

	/** 버프 효과의 연산 방식. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DSSettings | Buff")
	EOperationType OperationType;

	/** 고유 버프 ID. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DSSettings | Buff")
	int32 BuffID;

	/** 버프 값. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DSSettings | Buff")
	float BuffValue;

	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
	{
		Ar << StatType;
		Ar << OperationType;
		return true;
	}
};

template<>
struct TStructOpsTypeTraits<FBuffEntry> : public TStructOpsTypeTraitsBase2<FBuffEntry>
{
	enum
	{
		WithNetSerializer = true
	};
};

USTRUCT()
struct FBuffArray : public FFastArraySerializer
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(VisibleAnywhere, Category = "DSSettings | Buff")
    TArray<FBuffEntry> Entries;

	/** 다음 버프에 부여할 고유 ID */
	int32 NextBuffID;

	/** 각 버프의 제거 타이머 핸들을 저장하는 맵 (버프 ID -> 타이머 핸들) */
	TMap<int32, FTimerHandle> BuffTimerHandles;
	
	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FBuffEntry, FBuffArray>(Entries, DeltaParms, *this);
	}
};

template<>
struct TStructOpsTypeTraits<FBuffArray> : public TStructOpsTypeTraitsBase2<FBuffArray>
{
	enum
	{ 
		WithNetDeltaSerializer = true 
	};
};

/**
 * @brief 캐릭터의 스탯, 체력, 버프 관리 및 대미지/회복 처리를 담당하는 컴포넌트.
 */
UCLASS()
class PROJECT25L_API UDSStatComponent : public UPawnComponent, public IHealableInterface
{
	GENERATED_BODY()
	
public:
	friend struct FBuffArray;

	UDSStatComponent(const FObjectInitializer& ObjectInitializer);

	// StatUI를 관리하기 위한 델리게이트 선언
	DECLARE_MULTICAST_DELEGATE_TwoParams(FOnHPChanged, float CurrentHP, float MaxHP);
	FOnHPChanged OnHPChanged;

	/**
	 * @brief 기본 스탯 초기화 함수.
	 *
	 * 게임 데이터 서브시스템에서 캐릭터 스탯 데이터를 가져와 기본 스탯을 초기화합니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "DSSettings | Stats")
    void InitializeStats(EDataTableType InDataTableType, ECharacterType InCharacterType, int32 InNonCharacterID = -1);
	virtual void InitializeComponent() override;
protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/**
	 * @brief 현재 스탯(CurrentStat) 업데이트 함수.
	 *
	 * StatMemberMap의 모든 스탯에 대해 최종 스탯 값을 계산하여 CurrentStat를 업데이트한 후,
	 * 후처리 함수(PostUpdateCurrentStat)를 호출합니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "DSSettings | Stats")
	void UpdateCurrentStat();

	/**
	 * @brief 스탯 업데이트 후 호출되는 후처리 함수.
	 */
	UFUNCTION(BlueprintCallable, Category = "DSSettings | Stats")
	void PostUpdateCurrentStat();

	UFUNCTION()
	void OnRep_CurrentStat();

	UFUNCTION()
	void OnRep_HP();

public:
	/**
	 * @brief 기본 스탯 값 조회 함수.
	 *
	 * 지정된 스탯 유형에 해당하는 기본 스탯 값을 반환합니다.
	 *
	 * @param StatType 조회할 스탯 유형 (EDSStatType)
	 * @return 해당 스탯의 기본 값, 찾을 수 없으면 0.0f 반환
	 */
	UFUNCTION(BlueprintCallable, Category = "DSSettings | Stats")
	float GetDefaultStatByEnum(EDSStatType StatType) const;

	/**
	 * @brief 최종 스탯 값 계산 함수.
	 *
	 * 모든 활성 버프(ActiveBuffs)를 반영하여 최종 스탯 값을 계산합니다.
	 *
	 * @param StatType 계산할 스탯 유형 (EDSStatType)
	 * @return 계산된 최종 스탯 값 (음수일 경우 0 반환)
	 */
	UFUNCTION(BlueprintCallable, Category = "DSSettings | Stats")
	float GetFinalStat(EDSStatType StatType) const;

	/**
	 * @brief 버프 적용 함수.
	 *
	 * 지정된 스탯에 대해 버프를 적용한 후, 스탯을 업데이트합니다.
	 * 지속 시간이 지난 후 자동으로 제거되도록 타이머를 설정합니다.
	 *
	 * @param InStatType 적용할 스탯 유형
	 * @param InOperationType 버프의 연산 방식
	 * @param InBuffValue 버프 값
	 * @param InDuration 버프 지속 시간 (초)
	 */
	UFUNCTION(BlueprintCallable, Category = "DSSettings | Stats")
	void ApplyBuff(EDSStatType InStatType, EOperationType InOperationType, float InBuffValue, float InDuration);

	/**
	 * @brief 버프 제거 함수.
	 *
	 * 지정된 버프 ID에 해당하는 버프 항목을 제거한 후, 스탯을 업데이트합니다.
	 *
	 * @param InBuffID 제거할 버프의 고유 ID
	 */
	void RemoveBuff(int32 InBuffID);

public:
	FORCEINLINE const FDSCharacterStat& GetDefaultStat() const { return DefaultStat; }
	FORCEINLINE const FDSCharacterStat& GetCurrentStat() const { return CurrentStat; }
	FORCEINLINE float GetCurrentHP() const { return HP; }
	FORCEINLINE float GetMaxHP() const { return CurrentStat.MaxHP; }

	UFUNCTION(BlueprintCallable, Category = "DSSettings | Stats")
    void ModifyHP(float Delta);

	/**
	 * @brief 대미지 수신 처리 함수.
	 *
	 * 최종 대미지 계산 후 체력을 감소시킵니다.
	 *
	 * @param DamageAmount 입은 대미지 양
	 * @param DamageType 대미지 유형 (예: 기본 공격, 스킬)
	 * @param ElementType 원소 속성
	 * @param Instigator 대미지를 유발한 액터
	 *
	 */
	virtual void ReceiveDamage(float DamageAmount, EDamageType DamageType, EDSElementType ElementType, AActor* Instigator);

	/**
	 * @brief 체력 회복 처리 함수.
	 *
	 * 최종 회복량을 계산 후 체력을 증가시킵니다.
	 *
	 * @param HealAmount 회복량
	 * @param HealType 회복 유형 (예: 포션, 스킬)
	 * @param Instigator 회복을 유발한 액터
	 *
	 * @see IHealableInterface
	 */
	virtual void RecoverHealth_Implementation(float HealAmount, EHealType HealType, UObject* Instigator) override;

	float GetSkillDamageMultiplier(ESkillType SkillTag) const;
	float GetFinalSkillDamage(ESkillType SkillTag) const;


protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Transient, Category = "DSSettings | Stats")
    FDSCharacterStat DefaultStat;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Transient, ReplicatedUsing = OnRep_CurrentStat, Category = "DSSettings | Stats")
    FDSCharacterStat CurrentStat;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Transient, ReplicatedUsing = OnRep_HP, Category = "DSSettings | Stats")
	float HP;

	/** 활성화된 버프 항목들 */
	UPROPERTY(Replicated, VisibleAnywhere, Category = "DSSettings | Buff", Transient)
    FBuffArray ActiveBuffs;

	/** 다음 버프에 부여할 고유 ID */
	int32 NextBuffID;

	/** 각 버프의 제거 타이머 핸들을 저장하는 맵 (버프 ID -> 타이머 핸들) */
	UPROPERTY(Transient)
	TMap<int32, FTimerHandle> BuffTimerHandles;

	UPROPERTY(Transient, Replicated)
	ECharacterType CharacterType;
};