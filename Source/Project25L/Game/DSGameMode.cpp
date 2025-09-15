// Default
#include "Game/DSGameMode.h"

// UE
#include "GameFramework/Character.h"

// Game
#include "System/DSGameDataSubsystem.h"
#include "GameData/DSCharacterDataAsset.h"
#include "Player/DSPlayerController.h"
#include "Shader/DSCelShaderApplier.h"

#include "DSLogChannels.h"
void ADSGameMode::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	Super::PreLogin(Options, Address, UniqueId, ErrorMessage);

}

void ADSGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	UE_LOG(LogTemp, Warning, TEXT("[GameMode] PostLogin 호출됨"));

	// 셀 셰이딩 적용
	UDSCelShaderApplier::Initialize(this);
}

APawn* ADSGameMode::SpawnCharacter(APlayerController* InPlayerController, ECharacterType CharacterType)
{
	UDSGameDataSubsystem* DataSubsystem = UDSGameDataSubsystem::Get(this);

	check(DataSubsystem);

	APawn* OldPawn = InPlayerController->GetPawn();

	if(IsValid(OldPawn))
	{
		OldPawn->Destroy();
		InPlayerController->UnPossess();
	}

	AActor* StartSpot = ChoosePlayerStart(InPlayerController);

	if(IsValid(StartSpot))
	{
		const UDSCharacterDataAsset* DataAsset = DataSubsystem->GetDataAssetByType<ECharacterType, UDSCharacterDataAsset>(CharacterType);

		if (IsValid(DataAsset))
		{
			TSubclassOf<APawn> SpawnedPawnClass = DataAsset->ActorClass.LoadSynchronous();

			if(IsValid(SpawnedPawnClass))
			{
				UWorld* World = GetWorld();

				check(World);

				FActorSpawnParameters SpawnInfo;
				SpawnInfo.Instigator = GetInstigator();
				SpawnInfo.ObjectFlags |= RF_Transient;	// We never want to save default player pawns into a map
				
				APawn *NewPawn = World->SpawnActor<APawn>(SpawnedPawnClass, StartSpot->GetTransform(), SpawnInfo);
				
				if(IsValid(NewPawn))
				{
					InPlayerController->SetPawn(NewPawn);
					return NewPawn;
				}
			}
		}
	}

	return nullptr;
}
