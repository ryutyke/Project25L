
// Default
#include "System/DSEventSystems.h"

// UE

// Game
#include "System/DSGameInstance.h"

DSGameEvent GameEvent;
DSGameUI GameUI;
DSSkillInputEvent SkillInputEvent;

UDSEventSystems* UDSEventSystems::Get(UObject* Object)
{
	UWorld* World = Object->GetWorld();
	
	check(World);

	UDSGameInstance *GameInstance = Cast<UDSGameInstance>(World->GetGameInstance());

	check(GameInstance);

	return GameInstance->EventSystem;
}