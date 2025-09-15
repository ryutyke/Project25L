// Default
#include "DSChatPanel.h"

// UE
#include "Components/ListView.h"
#include "Components/EditableTextBox.h"
#include "Components/MultiLineEditableTextBox.h"
#include "Components/Button.h"

// Game
#include "Character/Characters/DSCharacter.h"
#include "Player/DSPlayerState.h"
#include "Player/DSPlayerController.h"

#include "GameData/Chat/DSChatData.h"
#include "Components/DSChatComponent.h"
#include "System/DSEventSystems.h"
#include "DSLogChannels.h"

UDSChatPanel::UDSChatPanel(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	
}

void UDSChatPanel::NativeConstruct()
{
	Super::NativeConstruct();

	if (IsValid(SendButton))
	{
		SendButton->OnClicked.AddDynamic(this, &UDSChatPanel::OnSendButtonClicked);
	}
}

void UDSChatPanel::BindEvents()
{
	ADSPlayerController* PlayerController = Cast<ADSPlayerController>(GetOwningPlayer());
	if(false == IsValid(PlayerController))
	{
		return;
	}

	UDSChatComponent* ChatComponent = PlayerController->GetChatComponent();

	if(false == IsValid(ChatComponent))
	{
		return;
	}
	DSEVENT_DELEGATE_BIND(ChatComponent->OnChatReceived, this, &UDSChatPanel::SendChatMessageEvent);
	DSEVENT_DELEGATE_BIND(OnChatSubmitted, ChatComponent, &UDSChatComponent::SendChatMessageToServer);
}

void UDSChatPanel::InitializeWidget()
{
	ADSPlayerController* PlayerController = Cast<ADSPlayerController>(GetOwningPlayer());
	if(false == IsValid(PlayerController))
	{
		return;
	}

	ADSPlayerState* PlayerState = Cast<ADSPlayerState>(PlayerController->PlayerState);
	if(false == IsValid(PlayerState))
	{
		return;
	}

	CurrentCharacterType = PlayerState->GetCharacterType();
}

void UDSChatPanel::OnSendButtonClicked()
{
	SendChatMessage();
}

void UDSChatPanel::SendChatMessage()
{
	if (false == IsValid(ChatInputBox)) 
	{
		return;
	}

	const FText Message = ChatInputBox->GetText();

	if (false == Message.IsEmpty())
	{
		// 나에게 메시지 보이기
		SendChatMessageEvent(CurrentCharacterType, Message);
		// 모두에게 메시지 보이기 
		DSEVENT_DELEGATE_INVOKE(OnChatSubmitted, Message);
		ChatInputBox->SetText(FText::GetEmpty());
	}
}

void UDSChatPanel::SendChatMessageEvent(const ECharacterType SenderType, const FText& Message)
{
	check(ChatListView);
	UDSChatData* ChatData = NewObject<UDSChatData>();

	if(IsValid(ChatData))
	{
		ChatData->CharacterType = SenderType;
		ChatData->ChatContent = Message;
		ChatListView->AddItem(ChatData);
	}
	
}
