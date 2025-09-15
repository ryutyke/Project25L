#pragma once

//Default
#include "CoreMinimal.h"

//UE
#include "UObject/NoExportTypes.h"
#include "Engine/NetSerialization.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "GameplayTagContainer.h"

// Game
#include "GameData/DSEnums.h"

// UHT
#include "DSSkillSpec.generated.h"

class UDSSkillBase;
class UDSSkillControlComponent;

USTRUCT()
struct FDSSkillActivationInfo
{
	GENERATED_BODY()
	FDSSkillActivationInfo()
		: ActivationMode(ESkillActivationMode::Authority)
		, bCanBeEndedByOtherInstance(false)
	{
	}

	FDSSkillActivationInfo(AActor* InActor);

	FDSSkillActivationInfo(ESkillActivationMode::Type InType)
		: ActivationMode(InType)
		, bCanBeEndedByOtherInstance(false)
	{
	}	

	UPROPERTY()
	mutable TEnumAsByte<ESkillActivationMode::Type>	ActivationMode;

	UPROPERTY()
	uint8 bCanBeEndedByOtherInstance:1;

	/** Called on client when activation is confirmed on server */
	void SetActivationConfirmed();

	/** Called when activation was rejected by the server */
	void SetActivationRejected();
};

/**
 * SkillSpec을 식별하기 위한 고유 핸들.
 * - 각 스킬은 고유한 Handle을 가지며, 이를 통해 외부에서 참조/요청할 수 있다.
 * - 구조체 자체는 경량이며, 비교/해시를 위한 연산자를 오버라이드한다.
 */

USTRUCT(BlueprintType)
struct PROJECT25L_API FDSSkillSpecHandle
{
	GENERATED_BODY()

	FDSSkillSpecHandle()
		: Handle(INDEX_NONE)
	{
	}

	/** 유효한 핸들인지 확인 */
	bool IsValid() const
	{
		return Handle != INDEX_NONE;
	}

	/** 새로운 핸들 생성 */
	void GenerateNewHandle();

	/** 비교 연산자 */
	bool operator==(const FDSSkillSpecHandle& Other) const { return Handle == Other.Handle; }
	bool operator!=(const FDSSkillSpecHandle& Other) const { return Handle != Other.Handle; }

	friend uint32 GetTypeHash(const FDSSkillSpecHandle& SpecHandle)
	{
		return ::GetTypeHash(SpecHandle.Handle);
	}

	FString ToString() const
	{
		return IsValid() ? FString::FromInt(Handle) : TEXT("Invalid");
	}

private:
	UPROPERTY()
	int32 Handle;
};


/**
 * 하나의 스킬 데이터를 정의하는 구조체
 * - Skill의 원형 클래스, 인스턴스 정보, 입력 정보 등을 포함
 * - FastArraySerializerItem을 상속받아 네트워크 복제 최적화 대상
 */
USTRUCT(BlueprintType)
struct PROJECT25L_API FDSSkillSpec : public FFastArraySerializerItem
{
	GENERATED_BODY()
	
	PRAGMA_DISABLE_DEPRECATION_WARNINGS
	FDSSkillSpec(const FDSSkillSpec&) = default;
	FDSSkillSpec(FDSSkillSpec&&) = default;
	FDSSkillSpec& operator=(const FDSSkillSpec&) = default;
	FDSSkillSpec& operator=(FDSSkillSpec&&) = default;
	~FDSSkillSpec() = default;
	PRAGMA_ENABLE_DEPRECATION_WARNINGS

	/** 기본 생성자 */
		FDSSkillSpec()
		: Skill(nullptr), InputID(INDEX_NONE), SourceObject(nullptr), ActiveCount(0), InputPressed(false), RemoveAfterActivation(false), PendingRemove(false), bActivateOnce(false)
	{ }

	/** 스킬 클래스를 이용한 생성자 */
	FDSSkillSpec(TSubclassOf<UDSSkillBase> InSkillClass,ESkillType InType, FGameplayTag InSkillTag, int32 InInputID = INDEX_NONE, UObject* InSourceObject = nullptr);

	/** 스킬 인스턴스를 직접 넘기는 생성자 */
	FDSSkillSpec(UDSSkillBase* InSkill, ESkillType InType,  FGameplayTag InSkillTag, int32 InInputID = INDEX_NONE, UObject* InSourceObject = nullptr);

	/** 외부 참조용 핸들 */
	UPROPERTY()
	FDSSkillSpecHandle Handle;
	
	/** 해당 스킬의 CDO (Class Default Object) */
	UPROPERTY()
	TObjectPtr<UDSSkillBase> Skill;
	
	/*
		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FGameplayTagContainer OwnedTags;
	*/

	/** 스킬 타입 */
	UPROPERTY()
	FGameplayTag SkillTag;

	/** 스킬 타입 */
	UPROPERTY()
	ESkillType SkillType;

	/** 이 스킬이 실행될 때 해당 태그가 있는 스킬을 강제 종료함 */
	UPROPERTY()
	FGameplayTagContainer CancelIfActiveTags;

	/** 이 태그가 현재 활성화된 스킬에 있다면, 이 스킬은 실행되지 않음 */
	UPROPERTY()
	FGameplayTagContainer BlockIfActiveTags;

	/** 스킬 활성화 정보*/
	UPROPERTY(NotReplicated)
	FDSSkillActivationInfo	ActivationInfo;

	/** 입력 ID (키 바인딩 또는 슬롯 인덱스 등과 연동) */
	UPROPERTY()
	int32	InputID;

	/** 이 스킬을 소유한 객체 (액터 혹은 데이터 객체 등) */
	UPROPERTY()
	TWeakObjectPtr<UObject> SourceObject;

	/** 활성화된 횟수 - 종료될 때 감소됨 */
	UPROPERTY(NotReplicated)
	uint8 ActiveCount;

	/** 입력이 현재 눌려있는 상태인지 */
	UPROPERTY(NotReplicated)
	uint8 InputPressed:1;

	/** 스킬 실행 후 제거될지 여부 */
	UPROPERTY(NotReplicated)
	uint8 RemoveAfterActivation:1;

	/** 제거 대기 중인지 여부 */
	UPROPERTY(NotReplicated)
	uint8 PendingRemove:1;

	/** 최초 1회만 실행될 스킬인지 여부 */
	UPROPERTY(NotReplicated)
	uint8 bActivateOnce : 1;

	/** 복제되지 않는 인스턴스들 */
	UPROPERTY(NotReplicated)
	TArray<TObjectPtr<UDSSkillBase>> NonReplicatedInstances;

	/** 복제되는 인스턴스들 */
	UPROPERTY()
	TArray<TObjectPtr<UDSSkillBase>> ReplicatedInstances;

	/** 인스턴스 중 우선 순위가 가장 높은 것을 반환 */
	UDSSkillBase* GetPrimaryInstance() const;

	/** 복제 여부 판단 */
	bool ShouldReplicateSkillSpec() const;

	/** 모든 스킬 인스턴스 반환 */
	TArray<UDSSkillBase*> GetSkillInstances() const
	{
		TArray<UDSSkillBase*> Skills;
		Skills.Append(ReplicatedInstances);
		Skills.Append(NonReplicatedInstances);
		return Skills;
	}

	/** 현재 스킬이 활성 상태인지 여부 */
	bool IsActive() const;

	/** 복제 제거 시 호출 */
	void PreReplicatedRemove(const struct FDSSkillSpecContainer& InArraySerializer);

	/** 복제 추가 시 호출 */
	void PostReplicatedAdd(const struct FDSSkillSpecContainer& InArraySerializer);

	/** 디버깅용 문자열 반환 */
	FString GetDebugString();
};


/**
 * FDSSkillSpec 배열을 FastArraySerializer 기반으로 감싸는 컨테이너
 * - UDSSkillControlComponent에서 관리
 * - NetDeltaSerialize를 통해 네트워크 최적화된 복제 가능
 */
USTRUCT(BlueprintType)
struct PROJECT25L_API FDSSkillSpecContainer : public FFastArraySerializer
{
	GENERATED_BODY()

	FDSSkillSpecContainer()
		: Owner(nullptr)
	{
	}

	/** 실제 스킬 데이터 배열 */
	UPROPERTY()
	TArray<FDSSkillSpec> Items;

	/** 이 컨테이너를 소유한 컴포넌트 */
	UPROPERTY(NotReplicated)
	TObjectPtr<UDSSkillControlComponent> Owner;

	/** 소유자 컴포넌트 등록 */
	void RegisterWithOwner(UDSSkillControlComponent* InOwner);

	/** FastArraySerializer 인터페이스 구현 - 델타 복제 수행 :  복제된 데이터만 전달 */
	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FDSSkillSpec, FDSSkillSpecContainer>(Items, DeltaParms, *this);
	}

	/**
	 * 복제 대상 항목인지 판단하는 조건
	 *
	 * @param bIsWritingOnClient : 현재 복제 데이터를 처리 중인 주체가 클라이언트인지 여부
	 * - 서버일 경우: 복제 대상 여부 판단
	 * - 클라이언트일 경우: 수신된 항목이 유효한지 확인
	 */
	template< typename Type, typename SerializerType >
	bool ShouldWriteFastArrayItem(const Type& Item, const bool bIsWritingOnClient)
	{
		// 이 스킬이 복제하지 않아도 된다면
		if (false == Item.ShouldReplicateSkillSpec())
		{
			// 복제 대상 제외
			return false;
		}

		// 클라이언트 입장에서 받은 복제 데이터를 처리하고 있는 중이라면
		if (bIsWritingOnClient)
		{
			// 유효한 ReplicationID를 가지고 있다면 복제 대상
			return Item.ReplicationID != INDEX_NONE;
		}

		// 서버는 위 조건만 통과하면 복제 대상
		return true;
	}
};

// FastArraySerializer 지원을 위한 Traits
template<>
struct TStructOpsTypeTraits< FDSSkillSpecContainer> : public TStructOpsTypeTraitsBase2< FDSSkillSpecContainer>
{
	enum
	{
		WithNetDeltaSerializer = true,
	};
};
