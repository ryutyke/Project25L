// Default
#include "System/DSGameInstance.h"

// UE

// Game
#include "System/DSEventSystems.h"
#include "System/DSGameUtils.h"
#include "Shader/DSCelShaderApplier.h"

UDSGameInstance::UDSGameInstance()
	: Super()
	, EventSystem(nullptr)
	, GameUtils(nullptr)
{
}

void UDSGameInstance::Init()
{
	Super::Init();

	EventSystem = NewObject<UDSEventSystems>();

	GameUtils = NewObject<UDSGameUtils>();

	UDSCelShaderApplier::Initialize(this);
}

void UDSGameInstance::Shutdown()
{
	Super::Shutdown();

	UDSCelShaderApplier::Shutdown();
}
