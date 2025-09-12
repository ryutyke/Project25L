#pragma once

// Defualt 
#include "CoreMinimal.h"

// Game
#include "Character/DSArmedCharacter.h"
#include "GameData/DSEnums.h"

// UHT
#include "DSCharacter_Girl.generated.h"

UCLASS()
class PROJECT25L_API ADSCharacter_Girl : public ADSArmedCharacter
{
	GENERATED_BODY()
	
public:
	ADSCharacter_Girl(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
