//Defualt
#include "Components/DSStatComponent.h"

//UE
#include "Net/UnrealNetwork.h"
#include "GameFramework/Character.h"

//Game
#include "System/DSGameDataSubsystem.h"
#include "DSLogChannels.h"
#include "GameData/Skill/DSSkillAttribute.h"
#include "Components/Skill/DSSkillControlComponent.h"

/**
 * @brief 각 스탯 유형과 FDSCharacterStat의 멤버 변수 포인터를 매핑하는 맵
 *
 * EDSStatType 열거형에 해당하는 스탯 멤버를 FDSCharacterStat 구조체에서 쉽게 접근하기 위한 매핑 테이블입니다.
 */
static const TMap<EDSStatType, float FDSCharacterStat::*> StatMemberMap =
{
	{EDSStatType::MaxHP,          &FDSCharacterStat::MaxHP},
	{EDSStatType::Attack,         &FDSCharacterStat::Attack},
	{EDSStatType::Defense,        &FDSCharacterStat::Defense},
	{EDSStatType::Luck,           &FDSCharacterStat::Luck},
	{EDSStatType::MoveSpeed,      &FDSCharacterStat::MoveSpeed},
	{EDSStatType::AttackSpeed,    &FDSCharacterStat::AttackSpeed},
	{EDSStatType::AttackRange,    &FDSCharacterStat::AttackRange},
	{EDSStatType::HPRegen,        &FDSCharacterStat::HPRegen},
	{EDSStatType::HPSteal,        &FDSCharacterStat::HPSteal},
	{EDSStatType::DamageBoost,    &FDSCharacterStat::DamageBoost},
	{EDSStatType::DamageReduction,&FDSCharacterStat::DamageReduction},
	{EDSStatType::CDReduction,    &FDSCharacterStat::CDReduction},
	{EDSStatType::CritChance,     &FDSCharacterStat::CritChance},
	{EDSStatType::CritDamage,     &FDSCharacterStat::CritDamage},
};

UDSStatComponent::UDSStatComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, NextBuffID(0)
{
	bWantsInitializeComponent = true;
}

void UDSStatComponent::InitializeComponent()
{
	Super::InitializeComponent();
}
void UDSStatComponent::BeginPlay()
{
	Super::BeginPlay();

	SetIsReplicated(true);
}

void UDSStatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UDSStatComponent, CurrentStat);
	DOREPLIFETIME(UDSStatComponent, HP);
	DOREPLIFETIME(UDSStatComponent, ActiveBuffs);
	DOREPLIFETIME(UDSStatComponent, CharacterType);
}

void UDSStatComponent::InitializeStats(EDataTableType InDataTableType, ECharacterType InCharacterType, int32 InNonCharacterID)
{
	CharacterType = InCharacterType;

	UDSGameDataSubsystem* DataSubsystem = UDSGameDataSubsystem::Get(this);
	check(DataSubsystem);

	FDSCharacterStat* CharacterStatData = nullptr;

	if (INDEX_NONE == InNonCharacterID)
	{
		//-1일 때 기본 캐릭터를 의미한다.
		CharacterStatData = DataSubsystem->GetDataRowByEnum<FDSCharacterStat, ECharacterType>(InDataTableType, InCharacterType);
	}
	else
	{
		CharacterStatData = static_cast<FDSCharacterStat*>(DataSubsystem->GetDataRowByID(InDataTableType, InNonCharacterID));
	}

	if (nullptr != CharacterStatData)
	{
		DefaultStat = *CharacterStatData;
		CurrentStat = DefaultStat;

		HP = CurrentStat.MaxHP;

		if (HasAuthority())
		{
			OnRep_CurrentStat();
			OnRep_HP();
		}
	}
	else
	{
		DS_LOG(DSStatLog, Warning, TEXT("Requested Character Data for type %s not found!"), *UEnum::GetValueAsString(InCharacterType));
	}
}

void UDSStatComponent::ModifyHP(float Delta)
{
	HP += Delta;
	
	if (HP > CurrentStat.MaxHP)
	{
		HP = CurrentStat.MaxHP;
		DS_LOG(DSStatLog, Log, TEXT("Over MaxHP"));
	}
	
	if (HP <= 0.f)
	{
		HP = 0.f;
		DS_LOG(DSStatLog, Log, TEXT("Die"));
	}
	
	OnRep_HP();

	DS_LOG(DSStatLog, Log, TEXT("[%s] [%s] HP 변경: %+f, 새 HP: %f"), *GetOwner()->GetName(), *GetName(), Delta, HP);
}

void UDSStatComponent::OnRep_CurrentStat()
{
	DS_NETLOG(DSStatLog, Log, TEXT("[%s] [%s] 새 HP: %f"), *GetOwner()->GetName(), *GetName(), HP);
}

void UDSStatComponent::OnRep_HP()
{
	DSEVENT_DELEGATE_INVOKE(OnHPChanged, HP, CurrentStat.MaxHP);
}


float UDSStatComponent::GetDefaultStatByEnum(EDSStatType StatType) const
{
	float FDSCharacterStat::* const* MemberPtr = StatMemberMap.Find(StatType);
	if (nullptr != MemberPtr)
	{
		return DefaultStat.*(*MemberPtr);
	}
	DS_LOG(DSStatLog, Warning, TEXT("Requested DefaultStat for type %s not found!"), *UEnum::GetValueAsString(StatType));
	return 0.0f;
}

float UDSStatComponent::GetFinalStat(EDSStatType StatType) const
{
	float TotalAddend = 0.f;
	float TotalMultiplier = 0.f;

	for (const FBuffEntry& Entry : ActiveBuffs.Entries)
	{
		if (Entry.StatType == StatType)
		{
			if (Entry.OperationType == EOperationType::Additive)
			{
				TotalAddend += Entry.BuffValue;
			}
			else if (Entry.OperationType == EOperationType::Multiplicative)
			{
				TotalMultiplier += Entry.BuffValue;
			}
			else if (Entry.OperationType == EOperationType::Percent)
			{
				TotalMultiplier += (Entry.BuffValue / 100);
			}
		}
	}

	// 최종 스탯 = (기본 스탯 + Additive 수치) * (1 + Multiplicative 수치)
	float DefaultStatValue = GetDefaultStatByEnum(StatType);
	float FinalStat = (DefaultStatValue + TotalAddend) * (1.f + TotalMultiplier);
	return FinalStat > 0.f ? FinalStat : 0.f;
}

void UDSStatComponent::UpdateCurrentStat()
{
	for (const TPair<EDSStatType, float FDSCharacterStat::*>& Pair : StatMemberMap)
	{
		const EDSStatType& StatType = Pair.Key;
		float FDSCharacterStat::* MemberPtr = Pair.Value;
		CurrentStat.*MemberPtr = GetFinalStat(StatType);
	}
	
	PostUpdateCurrentStat();
}

void UDSStatComponent::PostUpdateCurrentStat()
{
	// MaxHP 값 변경에 대한 HP 값 변경
	ModifyHP(0);
}

void UDSStatComponent::ApplyBuff(EDSStatType InStatType, EOperationType InOperationType, float InBuffValue, float InDuration)
{
	// 새로운 버프 항목 생성 및 추가
	FBuffEntry NewEntry(InStatType, InOperationType, NextBuffID++, InBuffValue);
	ActiveBuffs.Entries.Add_GetRef(MoveTemp(NewEntry));
	ActiveBuffs.MarkItemDirty(NewEntry);

	DS_LOG(DSStatLog, Log, TEXT("[%s] %s 버프 적용: ID=%d, %s %f, 지속시간: %f초"), *GetName(), *UEnum::GetValueAsString(InStatType), NewEntry.BuffID, *UEnum::GetValueAsString(InOperationType), InBuffValue, InDuration);

	// 버프 제거 타이머 설정
	FTimerHandle TimerHandle;
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindUObject(this, &UDSStatComponent::RemoveBuff, NewEntry.BuffID);

	UWorld* World = GetWorld();
	check(IsValid(World));
	if (IsValid(World))
	{
		World->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, InDuration, false);
		BuffTimerHandles.Add(NewEntry.BuffID, TimerHandle);
	}

	UpdateCurrentStat();
}

void UDSStatComponent::RemoveBuff(int32 InBuffID)
{
	// 버프 ID를 기준으로 해당 항목을 검색 후 제거
	int32 RemovedIndex = ActiveBuffs.Entries.IndexOfByPredicate([InBuffID](const FBuffEntry& Entry)
		{
			return Entry.BuffID == InBuffID;
		});

	if (RemovedIndex != INDEX_NONE)
	{
#if WITH_EDITOR
		float RemovedBuffValue = ActiveBuffs.Entries[RemovedIndex].BuffValue;
		EDSStatType RemovedBuffStatType = ActiveBuffs.Entries[RemovedIndex].StatType;
		EOperationType RemovedBuffOperationType = ActiveBuffs.Entries[RemovedIndex].OperationType;
		DS_LOG(DSStatLog, Log, TEXT("%s 버프 제거: ID=%d, %s %f"), *UEnum::GetValueAsString(RemovedBuffStatType), InBuffID, *UEnum::GetValueAsString(RemovedBuffOperationType), RemovedBuffValue);
#endif
		FBuffEntry& ItemToRemove = ActiveBuffs.Entries[RemovedIndex];
		ActiveBuffs.MarkItemDirty(ItemToRemove);
		ActiveBuffs.Entries.RemoveAt(RemovedIndex);
		BuffTimerHandles.Remove(InBuffID);

		UpdateCurrentStat();
	}
}

void UDSStatComponent::ReceiveDamage(float DamageAmount, EDamageType DamageType, EDSElementType ElementType, AActor* Instigator)
{
	ModifyHP(-DamageAmount);
}

void UDSStatComponent::RecoverHealth_Implementation(float HealAmount, EHealType HealType, UObject* Instigator)
{
	ModifyHP(HealAmount);
}

float UDSStatComponent::GetSkillDamageMultiplier(ESkillType SkillTag) const
{
	UDSGameDataSubsystem* DataSubsystem = UDSGameDataSubsystem::Get(this);
	check(DataSubsystem);

	EDataTableType DataTableType = UDSSkillControlComponent::GetSkillDataTableTypeByCharacter(CharacterType, INDEX_NONE);

	FDSSkillAttribute* SkillData;
	SkillData = DataSubsystem->GetDataRowByEnum<FDSSkillAttribute, ESkillType>(DataTableType, SkillTag);

	if (nullptr == SkillData)
	{
		DS_LOG(DSStatLog, Warning, TEXT("Skill Attribute Data not found!"));
		return 0.0f;
	}
	
	return SkillData->Damage;
}

float UDSStatComponent::GetFinalSkillDamage(ESkillType SkillTag) const
{
	// 스킬 데미지 = 공격력 * 스킬 배율 * 피해량 증가 배율
	
	float AttackStat = GetFinalStat(EDSStatType::Attack);
	float SkillMultiplier = GetSkillDamageMultiplier(SkillTag);
	float DamageMultiplier = 1.0f + GetFinalStat(EDSStatType::DamageBoost);
	
	float FinalDamage = AttackStat * SkillMultiplier * DamageMultiplier;

	/*UE_LOG(DSStatLog, Log, TEXT("Attack: %.2f, SkillMultiplier: %.2f, DamageBoost: %.2f, Final Damage: %.2f"),
		AttackStat, SkillMultiplier, DamageMultiplier, FinalDamage);*/

	return FinalDamage;
}
