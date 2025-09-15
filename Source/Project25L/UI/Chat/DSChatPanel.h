#pragma once

// Default
#include "CoreMinimal.h"


// Game
#include "UI/Base/DSUserWidget.h"
#include "GameData/DSEnums.h"

// UHT
#include "DSChatPanel.generated.h"

class UListView;
class UEditableTextBox;
class UMultiLineEditableTextBox;
class UButton;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnChatSubmitted, const FText& /*Message*/);

UCLASS()
class PROJECT25L_API UDSChatPanel : public UDSUserWidget
{
	GENERATED_BODY()

public:
	UDSChatPanel(const FObjectInitializer& ObjectInitializer);
	virtual void NativeConstruct() override;

	FOnChatSubmitted OnChatSubmitted;
	virtual void BindEvents() override;
	virtual void InitializeWidget() override;

private:
	UFUNCTION()
	void OnSendButtonClicked();

	void SendChatMessage();

	void SendChatMessageEvent(const ECharacterType SenderType, const FText& Message);

protected:
	UPROPERTY(meta = ( BindWidget ))
	TObjectPtr<UListView> ChatListView;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UMultiLineEditableTextBox> ChatInputBox;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> SendButton;


private:
	ECharacterType CurrentCharacterType;
};
