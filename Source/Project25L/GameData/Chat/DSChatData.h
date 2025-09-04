#pragma once

// Default
#include "CoreMinimal.h"

// UE
#include "UObject/NoExportTypes.h"

// Game
#include "GameData/DSEnums.h"

// UHT
#include "DSChatData.generated.h"


UCLASS()
class PROJECT25L_API UDSChatData : public UObject
{
	GENERATED_BODY()
	
public:
	UPROPERTY()
	ECharacterType CharacterType;

	UPROPERTY()
	FText ChatContent;
};