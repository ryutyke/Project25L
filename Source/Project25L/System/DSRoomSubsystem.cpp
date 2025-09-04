// Default
#include "System/DSRoomSubsystem.h"

// UE

// Game
#include "DSEventSystems.h"
#include "System/DSGameInstance.h"

UDSRoomSubsystem::UDSRoomSubsystem()
	: CurrentStage(EDungeonType::Max)
{
	
}

UDSRoomSubsystem* UDSRoomSubsystem::Get(UObject* Object)
{
	UWorld* World = Object->GetWorld();

	check(World);

	UDSGameInstance* GameInstance = Cast<UDSGameInstance>(World->GetGameInstance());

	check(GameInstance);

	return GameInstance->GetSubsystem<UDSRoomSubsystem>();
}

void UDSRoomSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UDSRoomSubsystem::InitializeStage()
{
	//현재 스팀에서 도전과제를 가져온다.

	//도전 과제에서 깬 Stage는 Chaos Destruction 수행할 수 있도록 DestroyDoor 호출
	DSEVENT_DELEGATE_INVOKE(GameEvent.OnDestroyStage,CurrentStage);
	//현재 과제 Stage만 UnlockDoor 호출
	DSEVENT_DELEGATE_INVOKE(GameEvent.OnUnlockStage,CurrentStage);
}

void UDSRoomSubsystem::ClearStage(EDungeonType NextStage)
{
	// 현재 Stage를 Chaos Destruction 수행
	DSEVENT_DELEGATE_INVOKE(GameEvent.OnDestroyStage, CurrentStage);

	// 다음 Stage 변경
	CurrentStage = NextStage;

	// 현재 Stage UnlockDoor 호출
	DSEVENT_DELEGATE_INVOKE(GameEvent.OnUnlockStage, CurrentStage);
}
