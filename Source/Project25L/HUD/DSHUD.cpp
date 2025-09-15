// Default
#include "HUD/DSHUD.h"

// UE
#include "Blueprint/UserWidget.h"

// Game
#include "UI/Base/DSPrimaryLayout.h"
#include "System/DSUIManagerSubsystem.h"


void ADSHUD::BeginPlay()
{
	Super::BeginPlay();
	InitializeWidgets();

}


void ADSHUD::InitializeWidgets()
{
	APlayerController* OwningPlayer = GetOwningPlayerController();

	if(false == IsValid(OwningPlayer))
	{
		return;
	}

	if(false == OwningPlayer->IsLocalController())
	{
		return;
	}

	if (PrimaryLayoutClass.Get() && OwningPlayer->IsLocalController())
	{ 
		UDSUIManagerSubsystem* UIManager = UDSUIManagerSubsystem::Get(this);
		check(UIManager);
		UDSPrimaryLayout* PrimaryLayout = CreateWidget<UDSPrimaryLayout>(OwningPlayer, PrimaryLayoutClass);
		ensure(PrimaryLayout != nullptr);
		if (true == IsValid(PrimaryLayout))
		{
			PrimaryLayout->AddToViewport(); 
			UIManager->RegisterWidget(PrimaryLayout);
			PrimaryLayout->BindEvents();
		}
	}
}



