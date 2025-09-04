
#pragma once

//Default
#include "CoreMinimal.h"

//UE
#include "GameFramework/CheatManager.h"
#include "GameplayTagContainer.h"

//UHT
#include "DSCheatManager.generated.h"

#ifndef USING_CHEAT_MANAGER
#define	USING_CHEAT_MANAGER (1 && !UE_BUILD_SHIPPING) 
#endif 


UCLASS()
class PROJECT25L_API UDSCheatManager : public UCheatManager
{
	GENERATED_BODY()
	
public:

	/**
	*  @brief 로그 그리고 콘솔에 입력된 텍스트 출력을 도와주는 치트
	*  @param TextToOutput	 출력할 텍스트
	*/
	static void CheatOutputText(const FString& TextToOutput);

	/**
	*  @brief 소유한 플레이에 대해 서버에서만 작동하는 치트
	*  @param Msg			 출력할 텍스트
	*/
	UFUNCTION(exec)
	void Cheat(const FString& Msg);

	/**
	*  @brief 모든 플레이에 대해 서버에서만 작동하는 치트
	*  @param Msg			출력할 텍스트
	*/
	UFUNCTION(exec)
	void CheatAll(const FString& Msg);

	UFUNCTION(exec)
	void CrouchMode(ECrouchMode CrouchMode);

	// --------------------------------------
	//	Skill Cheat Functions
	// --------------------------------------
	UFUNCTION(exec, BlueprintAuthorityOnly)
	void AddSkill(const int32 InputID);

	UFUNCTION(Exec, BlueprintAuthorityOnly)
	void ActivateSKill(const int32 InputID);
	
	UFUNCTION(Exec, BlueprintAuthorityOnly)
	void CancelSKill(const int32 InputID);

	UFUNCTION(Exec, BlueprintAuthorityOnly)
	void RemoveSKill(const int32 InputID);

	UFUNCTION(Exec, BlueprintAuthorityOnly, Category=Skill)
	void DisplaySkillCooltime(const int32 InputID);

	UFUNCTION(Exec, BlueprintAuthorityOnly, Category=Skill)
	void DisplaySkillCooltimeFromTag(const FGameplayTag SkillTag);

	/**
	*  @brief			아이템 보관하는 치트
	*  @param PlayerID  플레이어 아이디 (0부터 시작)
	*  @param ItemID	아이템 아이디 (DT_Item 테이블에 위치)
	*  @param ItemCount 아이템 개수 
	*/
	UFUNCTION(Exec, BlueprintAuthorityOnly)
	void StoreItems(int32 PlayerID, int32 ItemID, int32 ItemCount);
	/**
	*  @brief			아이템 사용하는 치트
	*  @param PlayerID  플레이어 아이디 (0부터 시작)
	*  @param ItemIdx	인벤토리에서 아이템의 위치
	*  @param ItemCount 아이템 개수
	*/
	UFUNCTION(Exec, BlueprintAuthorityOnly)
	void UseItems(int32 PlayerID, int32 ItemIdx, int32 ItemCount);
	
	
	UFUNCTION(Exec, BlueprintAuthorityOnly)
	void PrintPlayerItems(int32 PlayerID);
	
	UFUNCTION(Exec)
	void ApplyBuff(EDSStatType InStatType, EOperationType InOperationType, float InBuffValue, float InDuration);

	UFUNCTION(Exec)
	void ModifyHP(float Delta);
};
