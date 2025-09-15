// Default
#include "DSCombatLootWidget.h"

// Game
#include "UI/Chat/DSChatPanel.h"
#include "UI/Inventory/DSReadyBoardPanel.h"

UDSCombatLootWidget::UDSCombatLootWidget(const FObjectInitializer& ObjectInitializer)
:Super(ObjectInitializer)
{
}

void UDSCombatLootWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UDSCombatLootWidget::BindEvents()
{
	ChatPanel->BindEvents();
}

void UDSCombatLootWidget::InitializeWidget()
{
	ChatPanel->InitializeWidget();
	ReadyBoardPanel->InitializeWidget();
}
