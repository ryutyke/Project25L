#pragma once

// Default
#include "CoreMinimal.h"

// Game
#include "Components/Skill/DSSkillControlComponent.h"

// UHT
#include "DSSkillControlComponent_Mister.generated.h"

UCLASS()
class PROJECT25L_API UDSSkillControlComponent_Mister : public UDSSkillControlComponent
{
	GENERATED_BODY()
	
public:
	void AttackPrimarySkill(UAnimMontage* AttackAnimMontage, const FName& NextSection);

protected:
	UFUNCTION(Server, Reliable)
	void ServerRPC_AttackMontage(UAnimMontage* AttackAnimMontage, const FName& NextSection);

protected:

	UFUNCTION(Client, Reliable)
	void ClientRPC_AttackMontage(ADSCharacterBase* OtherPlayer,UAnimMontage* AttackAnimMontage, const FName& NextSection);

	void PlayMontage(UAnimMontage* AnimMontage, const FName& NextSection);

};
