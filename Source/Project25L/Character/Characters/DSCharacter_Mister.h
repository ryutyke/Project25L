#pragma once

// Default
#include "CoreMinimal.h"

// Game
#include "Character/Characters/DSCharacter.h"

// UHT
#include "DSCharacter_Mister.generated.h"

class USphereComponent;

UCLASS()
class PROJECT25L_API ADSCharacter_Mister : public ADSCharacter
{
	GENERATED_BODY()

public:
	ADSCharacter_Mister(const FObjectInitializer& ObjectInitializer);


protected:
	virtual void AttackHitCheck(const FName& HitSocket, const ESkillType& SkillTag) override;
};
