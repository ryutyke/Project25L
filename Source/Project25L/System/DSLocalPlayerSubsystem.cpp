// Default
#include "System/DSLocalPlayerSubsystem.h"

//UE
#include "Engine/GameInstance.h"
UDSLocalPlayerSubsystem* UDSLocalPlayerSubsystem::Get(UObject* Object)
{
	UWorld* World = Object->GetWorld();

	check(World);

	UGameInstance* GameInstance = Cast<UGameInstance>(World->GetGameInstance());

	check(GameInstance);

	return GameInstance->GetSubsystem<UDSLocalPlayerSubsystem>();
}
