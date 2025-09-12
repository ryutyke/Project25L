// Default
#include "System/DSAssetManager.h"

// UE

// Game
#include "GameData/GameplayTag/DSGameplayTags.h"

UDSAssetManager& UDSAssetManager::Get()
{
	check(GEngine);
	UDSAssetManager* DSAssetManager = Cast<UDSAssetManager>(GEngine->AssetManager);
	return *DSAssetManager;
}

void UDSAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();

	FDSTags::InitializeNativeGameplayTags();
}



