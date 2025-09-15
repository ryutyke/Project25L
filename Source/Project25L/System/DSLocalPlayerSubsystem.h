#pragma once

// Default
#include "CoreMinimal.h"

// UE
#include "Subsystems/GameInstanceSubsystem.h"

// Game
#include "GameData/DSEnums.h"

// UHT
#include "DSLocalPlayerSubsystem.generated.h"


UCLASS()
class PROJECT25L_API UDSLocalPlayerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	static UDSLocalPlayerSubsystem* Get(UObject* Object);

	void SetCharacterType(ECharacterType InCharacterType) { CharacterType = InCharacterType; }
	ECharacterType GetCharacterType() { return CharacterType; }
protected:

	UPROPERTY(Transient)
	ECharacterType CharacterType;

};
