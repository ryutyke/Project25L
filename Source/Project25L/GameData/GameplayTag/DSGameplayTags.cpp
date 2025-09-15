//Default
#include "GameData/GameplayTag/DSGameplayTags.h"

// UE
#include "GameplayTagsManager.h"

// Game
#include "GameData/DSEnums.h"
FDSTags FDSTags::GameplayTags;

void FDSTags::InitializeNativeGameplayTags()
{
	//********************************************************
	// Enhanced Input
	// Move
	GameplayTags.InputTag_Move = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.Move"));
	GameplayTags.InputTag_Look_Mouse = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.Look.Mouse"));
	GameplayTags.InputTag_Jump = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.Jump"));
	GameplayTags.InputTag_Parkour = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.Parkour"));
	GameplayTags.InputTag_FastRun = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.FastRun"));
	GameplayTags.InputTag_Sit = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.Sit"));

	// System
	GameplayTags.InputTag_Pause = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.Pause"));
	GameplayTags.InputTag_Interaction = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.Interaction"));
	GameplayTags.InputTag_Zoom = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.Zoom"));

	// Skill
	GameplayTags.InputTag_Skill_PrimarySkill = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.Skill.PrimarySkill"));
	GameplayTags.InputTag_Skill_SecondarySkill = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.Skill.SecondarySkill"));
	GameplayTags.InputTag_Skill_Skill1 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.Skill.Skill1"));
	GameplayTags.InputTag_Skill_Skill2 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.Skill.Skill2"));
	GameplayTags.InputTag_Skill_UltimateSkill = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.Skill.UltimateSkill"));
	GameplayTags.InputTag_Skill_FarmingSkill = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.Skill.FarmingSkill"));
	GameplayTags.InputTag_Skill_DodgeSkill = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.Skill.DodgeSkill"));

	// Item
	GameplayTags.InputTag_Item_QuickSlot1 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.Item.QuickSlot1"));
	GameplayTags.InputTag_Item_QuickSlot2 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.Item.QuickSlot2"));
	GameplayTags.InputTag_Item_QuickSlot3 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.Item.QuickSlot3"));
	GameplayTags.InputTag_Item_Vehicle = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.Item.Vehicle"));

	// Weapon
	GameplayTags.InputTag_Weapon_Toggle = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.Weapon.Toggle"));
	GameplayTags.InputTag_Weapon_Equipment_toggle = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.Weapon.Equipment_toggle"));

	// UI
	GameplayTags.InputTag_UI_Inventory = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.UI.Inventory"));
	GameplayTags.InputTag_UI_Status = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.UI.Status"));

	// FilghtSkill
	GameplayTags.InputTag_Skill_Flight_Begin = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.Skill.Flight.Begin"));
	GameplayTags.InputTag_Skill_Flight_Up = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.Skill.Flight.Up"));
	GameplayTags.InputTag_Skill_Flight_Down = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.Skill.Flight.Down"));

	//********************************************************
	// UI Layer
	GameplayTags.UI_Layer_GameplayUI = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("UI.Layer.GameplayUI"));
	GameplayTags.UI_Layer_Modal = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("UI.Layer.Modal"));
	GameplayTags.UI_Layer_Persistent = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("UI.Layer.Persistent"));
	GameplayTags.UI_Layer_Persistent_Main = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("UI.Layer.Persistent.Main"));

	GameplayTags.UI_Layer_GameplayUI_CombatInventoryWidget = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("UI.Layer.GameplayUI.CombatInventoryWidget"));
	GameplayTags.UI_Layer_GameplayUI_CombatLootWidget = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("UI.Layer.GameplayUI.CombatLootWidget"));
	

	//********************************************************
	// Skill
	GameplayTags.Skill_PrimarySkill = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Skill.PrimarySkill"));
	GameplayTags.Skill_SecondarySkill = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Skill.SecondarySkill"));
	GameplayTags.Skill_Skill1 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Skill.Skill1"));
	GameplayTags.Skill_Skill2 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Skill.Skill2"));
	GameplayTags.Skill_FarmingSkill = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Skill.FarmingSkill"));
	GameplayTags.Skill_UltimateSkill = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Skill.UltimateSkill"));
	GameplayTags.Skill_TestSkill = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Skill.TestSkill"));
	GameplayTags.Skill_None = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Skill.NoneSkill"));
	GameplayTags.Skill_DodgeSkill = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Skill.DodgeSkill"));
	

	GameplayTags.SkillType.Add({ GameplayTags.InputTag_Skill_PrimarySkill, ESkillType::PrimarySkill });
	GameplayTags.SkillType.Add({ GameplayTags.InputTag_Skill_SecondarySkill, ESkillType::SecondarySkill });
	GameplayTags.SkillType.Add({ GameplayTags.InputTag_Skill_Skill1, ESkillType::Skill1 });
	GameplayTags.SkillType.Add({ GameplayTags.InputTag_Skill_Skill2, ESkillType::Skill2 });
	GameplayTags.SkillType.Add({ GameplayTags.InputTag_Skill_UltimateSkill, ESkillType::UltimateSkill });
	GameplayTags.SkillType.Add({ GameplayTags.InputTag_Skill_FarmingSkill, ESkillType::FarmingSkill });
	GameplayTags.SkillType.Add({ GameplayTags.InputTag_Skill_DodgeSkill, ESkillType::DodgeSkill });


	GameplayTags.SkillType.Add({ GameplayTags.Skill_PrimarySkill, ESkillType::PrimarySkill });
	GameplayTags.SkillType.Add({ GameplayTags.Skill_SecondarySkill, ESkillType::SecondarySkill });
	GameplayTags.SkillType.Add({ GameplayTags.Skill_Skill1, ESkillType::Skill1 });
	GameplayTags.SkillType.Add({ GameplayTags.Skill_Skill2, ESkillType::Skill2 });
	GameplayTags.SkillType.Add({ GameplayTags.Skill_UltimateSkill, ESkillType::UltimateSkill });
	GameplayTags.SkillType.Add({ GameplayTags.Skill_FarmingSkill, ESkillType::FarmingSkill });
	GameplayTags.SkillType.Add({ GameplayTags.Skill_DodgeSkill, ESkillType::DodgeSkill });

}
