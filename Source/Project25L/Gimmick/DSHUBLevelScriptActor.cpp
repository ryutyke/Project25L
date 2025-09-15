// Default
#include "Gimmick/DSHUBLevelScriptActor.h"

// UE
#include "Kismet/GameplayStatics.h"

// Game
#include "Gimmick/DSDungeonDoor.h"
#include "System/DSRoomSubsystem.h"

ADSHUBLevelScriptActor::ADSHUBLevelScriptActor()
	: Super()
{
}

void ADSHUBLevelScriptActor::BeginPlay()
{
	Super::BeginPlay();

	/*실제 월드에 문이 전부 다 설치 되었는지 확인할 타이머를 설치한다.*/
	UWorld* World = GetWorld();
	check(World);
	World->GetTimerManager().SetTimerForNextTick(this, &ThisClass::CheckSpawnedDoor);

}

void ADSHUBLevelScriptActor::CheckSpawnedDoor()
{
	UWorld* World = GetWorld();

	check(World);

	TArray<AActor*> ActorsToFind;

	//'FindActorTag'가 있는 FireEffect 클래스의 모든 액터를 가져옵니다.
	UGameplayStatics::GetAllActorsOfClassWithTag(World, ADSDungeonDoor::StaticClass(), FName("DoorTag"), ActorsToFind);

	if (ActorsToFind.Num() < DoorMaxCount)
	{
		//모두 스폰 안됐음.
		World->GetTimerManager().SetTimerForNextTick(this, &ThisClass::CheckSpawnedDoor);
		return;
	}

	//모두 스폰되었다.
	//이벤트를 발동 (델리게이트 Invoke)

	UDSRoomSubsystem* RoomSubsystem = UDSRoomSubsystem::Get(this);

	check(RoomSubsystem);

	RoomSubsystem->InitializeStage();
}
