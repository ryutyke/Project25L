// Default
#include "DSSkillBase_Mister.h"

// UE
#include "GameFramework/Character.h"

// Game
#include "Components/Skill/DSSkillControlComponent.h"
#include "System/DSGameDataSubsystem.h"
#include "GameData/Skill/DSSkillAttribute.h"
UDSSkillBase_Mister::UDSSkillBase_Mister(const FObjectInitializer& ObjectInitalize)
	:Super(ObjectInitalize)
{
}

void UDSSkillBase_Mister::OnSkillInitialized()
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

	const ECharacterType CharacterType = ECharacterType::Mister;
	const EDataTableType DataTableType = UDSSkillControlComponent::GetSkillDataTableTypeByCharacter(CharacterType, -1);

	UDSGameDataSubsystem* DataManager = UDSGameDataSubsystem::Get(Character);
	check(DataManager);

	FDSSkillAttribute* SkillData = DataManager->GetDataRowByEnum<FDSSkillAttribute, ESkillType>(DataTableType, CurrentSkillType);


}
