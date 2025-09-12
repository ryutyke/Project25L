#pragma once
// Default
#include "CoreMinimal.h"

// Game
#include "UI/Base/DSUserWidget.h"

// UHT
#include "DSCombatLootWidget.generated.h"

class UDSChatPanel;
class UDSCombatInventoryWidget;
class UDSTeamInventoryGridPanel;
class UDSTimerPanel;
class UDSReadyBoardPanel;

UCLASS()
class PROJECT25L_API UDSCombatLootWidget : public UDSUserWidget
{
	GENERATED_BODY()
	
public:
	UDSCombatLootWidget(const FObjectInitializer& ObjectInitializer);
	virtual void NativeConstruct() override;
	virtual void BindEvents() override;
	virtual void InitializeWidget() override;

protected:
	UPROPERTY(meta = ( BindWidget ))
	TObjectPtr<UDSCombatInventoryWidget> PersonalInventoryPanel;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UDSTeamInventoryGridPanel> TeamInventoryPanel;

	UPROPERTY(meta = ( BindWidget ))
	TObjectPtr<UDSChatPanel> ChatPanel;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UDSTimerPanel> TimerPanel;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UDSReadyBoardPanel> ReadyBoardPanel;
};
