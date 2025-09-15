// Default
#include "DSSkillBase_Girl.h"

// UE
#include "GameFramework/Character.h"

// Game
#include "Components/Skill/DSSkillControlComponent.h"
#include "System/DSGameDataSubsystem.h"
#include "GameData/Skill/DSSkillAttribute.h"

UDSSkillBase_Girl::UDSSkillBase_Girl(const FObjectInitializer& ObjectInitalize)
	:Super(ObjectInitalize)
	, SkillDistance(0.f)
	, SkillWaitTime(0.f)
{
}

void UDSSkillBase_Girl::OnSkillInitialized()
{
	Super::OnSkillInitialized();

	if (nullptr == CurrentActorInfo)
	{
		return;
	}

	ACharacter* Character = Cast<ACharacter>(CurrentActorInfo->SkillOwner.Get());

	if (false == IsValid(Character))
	{
		return;
	}

	const ECharacterType CharacterType = ECharacterType::Girl;
	const EDataTableType DataTableType = UDSSkillControlComponent::GetSkillDataTableTypeByCharacter(CharacterType, -1);

	UDSGameDataSubsystem* DataManager = UDSGameDataSubsystem::Get(Character);
	check(DataManager);

	FDSSkillAttribute_Girl* SkillData = DataManager->GetDataRowByEnum<FDSSkillAttribute_Girl, ESkillType>(DataTableType, CurrentSkillType);

	if (nullptr != SkillData)
	{
		SkillDistance = SkillData->SKillDistance;
		SkillWaitTime = SkillData->SkillWaitTime;
	}
}
