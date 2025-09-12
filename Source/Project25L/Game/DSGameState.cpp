// Default
#include "Game/DSGameState.h"

// UE
#include "GameFramework/PlayerState.h"
#include "Net/UnrealNetwork.h"

// Game
#include "Character/Characters/DSCharacter.h"
#include "System/DSEventSystems.h"
#include "DSLogChannels.h"

ADSGameState::ADSGameState()
	: Super()
	, SpawnedPlayerCount(0)
	, bIsSpawned(false)
{
}

void ADSGameState::AddPlayerState(APlayerState* PlayerState)
{
	Super::AddPlayerState(PlayerState);

	if (HasAuthority())
	{
		SpawnedPlayerCount++;

		// 서버 환경에서 검사
		if (SpawnedPlayerCount >= MaxPlayerCount)
		{
			FTimerHandle WaitTimerHanle;

			UWorld* World = GetWorld();
			check(World);
			World->GetTimerManager().SetTimer(WaitTimerHanle, this, &ThisClass::CheckSpawnedCharacter, 3.f);
		}

	}
}

void ADSGameState::OnRep_SpawnedPlayerCount()
{
	//OnRep으로 들어온 SpawnedPlayerCount를 수 세고,
	if (SpawnedPlayerCount >= MaxPlayerCount)
	{
		FTimerHandle WaitTimerHanle;

		UWorld* World = GetWorld();
		check(World);
		World->GetTimerManager().SetTimer(WaitTimerHanle, this, &ThisClass::CheckSpawnedCharacter, 3.f);
	}
}

void ADSGameState::CheckSpawnedCharacter()
{

	if (bIsSpawned)
	{
		return;
	}

	UWorld* World = GetWorld();
	check(World);
	
	//아직 PlayerArray가 초기화되지 않았을 경우에, 다음 틱에서 수행
	if (PlayerArray.Num() < MaxPlayerCount)
	{
		FTimerHandle WaitTimerHanle;
		World->GetTimerManager().SetTimer(WaitTimerHanle, this, &ThisClass::CheckSpawnedCharacter, 3.f);
		return;
	}

	bool bAllCharactersReady = true;
	//모든 플레이어를 가지고온다.
	for (const auto PlayerState : PlayerArray)
	{
		ADSCharacter* Character = PlayerState->GetPawn<ADSCharacter>();

		if (false == IsValid(Character))
		{
			bAllCharactersReady = false;
		}
	}

	// Pawn이 준비되지 않았을 경우 준비가 안됐음을 의미
	if (false == bAllCharactersReady)
	{
		FTimerHandle WaitTimerHanle;
		World->GetTimerManager().SetTimer(WaitTimerHanle, this, &ThisClass::CheckSpawnedCharacter, 3.f);
	}
	else
	{
		for (const auto PlayerState : PlayerArray)
		{
			ADSCharacter* Character = PlayerState->GetPawn<ADSCharacter>();

			if (IsValid(Character))
			{
				//모든 캐릭터가 스폰되었다는 델리게이트를 달아도 된다.
				static int32 Count = 0;
				DS_LOG(DSLog, Log, TEXT("%s %d"), *Character->GetName(), Count);
				bIsSpawned = true;
				DSEVENT_DELEGATE_INVOKE(GameEvent.OnCharacterSpawned);
				Character->InitializeUI();
			}
		}

	}
}
void ADSGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADSGameState, SpawnedPlayerCount);
}
