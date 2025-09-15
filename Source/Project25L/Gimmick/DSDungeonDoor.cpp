// Default
#include "Gimmick/DSDungeonDoor.h"

// UE
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "EngineUtils.h"
#include "LevelSequenceActor.h"
#include "LevelSequencePlayer.h"

// Game
#include "DSLogChannels.h"
#include "GameData/Gimmick/DSDoorData.h"
#include "Gimmick/DSBead.h"
#include "Gimmick/DSDungeonMat.h"
#include "System/DSEventSystems.h"

ADSDungeonDoor::ADSDungeonDoor()
	: Super()
	, MatVolume(nullptr)
	, bIsUnlocked(false)
{
	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Border = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Border"));
	
	RootComponent = Trigger;
	Mesh->SetupAttachment(Trigger);
	Border->SetupAttachment(Mesh);

	Tags.Add(TEXT("DoorTag"));
}

void ADSDungeonDoor::UnlockDoor(EDungeonType CurrentType)
{
	if (IsValid(DungeonDoorData))
	{
		if (DungeonDoorData->DungeonType != CurrentType)
		{
			return;
		}

		bIsUnlocked = true;
	}

}

void ADSDungeonDoor::TryDestroyDoor(EDungeonType CurrentType)
{
	if (false == IsValid(DungeonDoorData))
	{
		return;
	}

	if (DungeonDoorData->DungeonType >= CurrentType)
	{
		return;
	}

	EDungeonType Type = DungeonDoorData->DungeonType;
	DS_LOG(DSItemLog, Log, TEXT("%s %s"), *UEnum::GetValueAsString(Type), *UEnum::GetValueAsString(CurrentType));
	if (IsValid(MatVolume))
	{
		MatVolume->UnbindEvents();
	}

	UWorld* World = GetWorld();

	if (IsValid(World))
	{
		ensure(World);
		ALevelSequenceActor* SequenceActor;
		FMovieSceneSequencePlaybackSettings PlaybackSettings;
		PlaybackSettings.FinishCompletionStateOverride = EMovieSceneCompletionModeOverride::ForceKeepState; // 상태 복원 금지 (기존 상태로 되돌리지 않음)

		ULevelSequencePlayer* LevelSequencePlayer = ULevelSequencePlayer::CreateLevelSequencePlayer(
			World, DungeonDoorData->DestroyDoorSequence.LoadSynchronous(), PlaybackSettings, SequenceActor);

		if (IsValid(LevelSequencePlayer))
		{
			LevelSequencePlayer->Play();
			LevelSequencePlayer->OnFinished.AddDynamic(this, &ADSDungeonDoor::DestroyDoor);

			//구슬을 스폰한다. => Sequence 가 실행되었을 때 구슬을 스폰한다.

			if (IsValid(DungeonDoorData))
			{
				ADSBead* Bead = World->SpawnActor<ADSBead>(DungeonDoorData->Bead.LoadSynchronous(), GetActorTransform());

				if (IsValid(Bead))
				{
					Bead->InitializeMaterial(DungeonDoorData->ChangedBeadMateral);
				}
			}
		}
	}
}

void ADSDungeonDoor::InitializeStage()
{

	if (false == IsValid(DungeonDoorData))
	{
		return;
	}

	UWorld* World = GetWorld();

	check(World);

	// World를 사용해서 Iterator 돌려서 Actor가져올 수 있도록 변경하기
	for (TActorIterator<ADSDungeonMat> It(World, ADSDungeonMat::StaticClass()); It; ++It)
	{
		ADSDungeonMat* DungeonMat = *It;

		if (IsValid(DungeonMat))
		{
			if (DungeonMat->GetDungeonDoorType() == DungeonDoorData->DungeonType)
			{
				MatVolume = DungeonMat;
				break;
			}
		}
	}

	if (bIsUnlocked)
	{
		if (IsValid(MatVolume))
		{
			MatVolume->BindEvents();
		}
	}

	DSEVENT_DELEGATE_BIND(GameEvent.OnUnlockStage, this, &ADSDungeonDoor::UnlockDoor);
	DSEVENT_DELEGATE_BIND(GameEvent.OnDestroyStage, this, &ADSDungeonDoor::TryDestroyDoor);
}

void ADSDungeonDoor::DestroyDoor()
{
	//해당 Door을 삭제하고,
	Destroy();
}

void ADSDungeonDoor::BeginPlay()
{
	Super::BeginPlay();

	InitializeStage();
}

void ADSDungeonDoor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	DSEVENT_DELEGATE_REMOVE(GameEvent.OnUnlockStage, this);
	DSEVENT_DELEGATE_REMOVE(GameEvent.OnDestroyStage, this);
}
