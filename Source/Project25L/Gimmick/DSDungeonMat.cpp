// Default
#include "Gimmick/DSDungeonMat.h"

// UE
#include "Components/BoxComponent.h"

// Game
#include "Character/Characters/DSCharacter.h"
#include "GameData/Gimmick/DSDoorData.h"
#include "Player/DSPlayerController.h"
#include "System/DSEventSystems.h"

#include "DSLogChannels.h"

ADSDungeonMat::ADSDungeonMat()
	: Super()
	, MaxPlayers(3)
	, WaitSec(3.f)
{
	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));

	RootComponent = Trigger;
}

void ADSDungeonMat::BindEvents()
{
	if (IsValid(Trigger))
	{
		Trigger->OnComponentBeginOverlap.RemoveAll(this);
		Trigger->OnComponentBeginOverlap.AddDynamic(this, &ADSDungeonMat::OnComponentBeginOverlap);
		Trigger->OnComponentEndOverlap.RemoveAll(this);
		Trigger->OnComponentEndOverlap.AddDynamic(this, &ADSDungeonMat::OnComponentEndOverlap);
	}
}

void ADSDungeonMat::UnbindEvents()
{
	if (IsValid(Trigger))
	{
		Trigger->OnComponentBeginOverlap.RemoveAll(this);
		Trigger->OnComponentEndOverlap.RemoveAll(this);
	}
}

void ADSDungeonMat::InitializeData(const FString& DoorDestination)
{
	Destination = DoorDestination;
}


void ADSDungeonMat::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ADSCharacter* Character = Cast<ADSCharacter>(OtherActor);

	if (IsValid(Character))
	{
		OverlappedPlayers.Add(Character);
		// 오버랩된 플레이어가 비어있다면, 처음 입장한 플레이어로 인식되어진다.
		if (Character->HasAuthority())
		{
			//UI를 띄운다.
			Character->ServerRPC_ReadyPlayer(false, OverlappedPlayers.Num(), ReadyPlayerWidgetTag);
		}

		UWorld* World = GetWorld();

		check(World);

		if (OverlappedPlayers.Num() == MaxPlayers)
		{
			DS_NETLOG(DSNetLog, Log, TEXT("The timer works %d %d"),OverlappedPlayers.Num(), MaxPlayers);
			//시간초가 작동한다.
			World->GetTimerManager().SetTimer(WaitTimer, this, &ADSDungeonMat::PrepareDungeonTravel, WaitSec, false);
		}
	}
}

void ADSDungeonMat::OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

	ADSCharacter* Character = Cast<ADSCharacter>(OtherActor);

	if (IsValid(Character))
	{
		UWorld* World = GetWorld();

		check(World);

		//한 명의 플레이어라도 나갈 경우 타이머를 초기화한다.
		World->GetTimerManager().ClearTimer(WaitTimer);

		for (const auto OverlappedPlayer : OverlappedPlayers)
		{
			if (Character == OverlappedPlayer)
			{
				OverlappedPlayers.Remove(Character);
				break;
			}
		}

		bool bCanClose = false;
		if (Character->HasAuthority())
		{
			if (OverlappedPlayers.Num() == 0)
			{
				//비어있으면, 모든 플레이어가 나갔기 때문에 UI를 없앤다.
				bCanClose = true;
			}

			Character->ServerRPC_ReadyPlayer(bCanClose, OverlappedPlayers.Num(), ReadyPlayerWidgetTag);
		}
	}
}

void ADSDungeonMat::PrepareDungeonTravel()
{

	// Travel로 이동하기 전에 Cinematic or Animation 재생 되어진다.
	// PlayerController로부터 부탁하여 재생될 수 있도록 한다.
	// Cinematic or Animation이 끝날 때 실제 ServerTravel이 이루어진다.

	for (const auto OverlappedPlayer : OverlappedPlayers)
	{
		ADSPlayerController* PlayerController = OverlappedPlayer->GetController<ADSPlayerController>();

		if (IsValid(PlayerController))
		{
			// UI 보이게 설정
		}
	}

	if (HasAuthority())
	{
		UWorld* World = GetWorld();

		check(World);

		FTimerHandle TravelWaitTimerHandle;

		GetWorld()->GetTimerManager().SetTimer(TravelWaitTimerHandle, this, &ADSDungeonMat::TravelDungeon, 5.0f, false);
	}

}

void ADSDungeonMat::TravelDungeon()
{
	UWorld* World = GetWorld();

	check(World);

	World->ServerTravel(Destination);

}

