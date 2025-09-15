#pragma once

// Default
#include "CoreMinimal.h"

// Game
#include "Character/DSArmedCharacter.h"

// UHT
#include "DSCharacter_Boy.generated.h"

UCLASS()
class PROJECT25L_API ADSCharacter_Boy : public ADSArmedCharacter
{
	GENERATED_BODY()
	
public:
	ADSCharacter_Boy(const FObjectInitializer& ObjectInitializer);

	void BeginPrimarySkill();

	void JumpToNextPrimaryAttackCombo(int32 MaxSequenceCount);
	
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_JumpToNextPrimaryAttackCombo(int32 NextSectionIndex);

	virtual void ProgressCombo(const ESkillType& SkillTag, int32 MaxSequenceCount) override;

protected:
	void InitComboAttack();
	UFUNCTION(Server, Reliable)
	void ServerRPC_InitComboAttack();
	void EndCombo(const ESkillType& SkillTag);
};
