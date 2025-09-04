// Default
#include "Item/DSItemPotion.h"

// Game
#include "DSLogChannels.h"
#include "GameData/Items/DSItemStatData.h"
#include "Components/DSStatComponent.h"
#include "Character/Characters/DSCharacter.h"
#include "Components/DSFlightComponent.h"
#include "System/DSEventSystems.h"

UDSItemPotion::UDSItemPotion()
	: Super()
	, Duration(0.f)
	, BoostHP(0.f)
	, AddMaxHP(0.f)
	, AddAttack(0.f)
	, AddDefense(0.f)
	, AddLuck(0.f)
	, AddMoveSpeed(0.f)
	, AddJumpHeight(0.f)
	, AddAttackSpeed(0.f)
	, AddHPRegen(0.f)
	, AddHPSteal(0.f)
	, AddAttackRange(0.f)
	, AddDamageBoost(0.f)
	, bCanFly(false)
{
}

void UDSItemPotion::Initialize(FTableRowBase* DataTableRow)
{
	FDSItemStatData* ItemDataPtr = static_cast<FDSItemStatData*>(DataTableRow);
	
	if (nullptr != ItemDataPtr)
	{
		Duration = ItemDataPtr->ItemDuration;
		BoostHP = ItemDataPtr->BoostHP;
		AddMaxHP = ItemDataPtr->AddMaxHP;
		AddAttack = ItemDataPtr->AddAttack;
		AddDefense = ItemDataPtr->AddDefense;
		AddLuck = ItemDataPtr->AddLuck;
		AddMoveSpeed = ItemDataPtr->AddMoveSpeed;
		AddJumpHeight = ItemDataPtr->AddJumpHeight;
		AddAttackSpeed = ItemDataPtr->AddAttackSpeed;
		AddHPRegen = ItemDataPtr->AddHPRegen;
		AddHPSteal = ItemDataPtr->AddHPSteal;
		AddAttackRange = ItemDataPtr->AddAttackRange;
		AddDamageBoost = ItemDataPtr->AddDamageBoost;
		bCanFly = ItemDataPtr->bCanFly;
	}
	else
	{
		DS_LOG(DSItemLog, Warning, TEXT("Invalid Potion Item DataTableRow"));
	}
}

void UDSItemPotion::UseItem(UDSStatComponent* Stat)
{
	// 아이템 사용에 대한 정의 합니다.
	DS_LOG(DSItemLog, Log, TEXT("Use Item Potion"));
	if (BoostHP != 0.f)
	{
		Stat->RecoverHealth(BoostHP, EHealType::Potion, this);
	}
	
	if (Duration != 0.f)
	{
		if (AddMaxHP != 0.f)
		{
			Stat->ApplyBuff(EDSStatType::MaxHP, EOperationType::Additive, AddMaxHP, Duration);
		}
		if (AddAttack != 0.f)
		{
			Stat->ApplyBuff(EDSStatType::Attack, EOperationType::Additive, AddAttack, Duration);
		}
		if (AddDefense != 0.f)
		{
			Stat->ApplyBuff(EDSStatType::Defense, EOperationType::Additive, AddDefense, Duration);
		}
		if (AddLuck != 0.f)
		{
			Stat->ApplyBuff(EDSStatType::Luck, EOperationType::Additive, AddLuck, Duration);
		}
		if (AddMoveSpeed != 0.f)
		{
			Stat->ApplyBuff(EDSStatType::MoveSpeed, EOperationType::Additive, AddMoveSpeed, Duration);
		}
		if (AddJumpHeight != 0.f)
		{
			// Todo
		}
		if (AddAttackSpeed != 0.f)
		{
			Stat->ApplyBuff(EDSStatType::AttackSpeed, EOperationType::Additive, AddAttackSpeed, Duration);
		}
		if (AddHPRegen != 0.f)
		{
			Stat->ApplyBuff(EDSStatType::HPRegen, EOperationType::Additive, AddHPRegen, Duration);
		}
		if (AddHPSteal != 0.f)
		{
			Stat->ApplyBuff(EDSStatType::HPSteal, EOperationType::Additive, AddHPSteal, Duration);
		}
		if (AddAttackRange != 0.f)
		{
			Stat->ApplyBuff(EDSStatType::AttackRange, EOperationType::Additive, AddAttackRange, Duration);
		}
		if (AddDamageBoost != 0.f)
		{
			Stat->ApplyBuff(EDSStatType::DamageBoost, EOperationType::Additive, AddDamageBoost, Duration);
		}
		if (bCanFly)
		{
			if(false == IsValid(Stat->GetOwner()))
			{
				return;
			}
			ADSCharacter* Character = Cast<ADSCharacter>(Stat->GetOwner());
			
			if(false == IsValid(Character))
			{
				return;
			}

			Character->SetCanFly(true);
		}
	}
	
}
