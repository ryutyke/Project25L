// Default
#include "System/DSUIManagerSubsystem.h"

// UE
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameplayTagContainer.h"

// Game
#include "DSLogChannels.h"
#include "Player/DSPlayerController.h"
#include "System/DSGameInstance.h"
#include "UI/Base/DSPrimaryLayout.h"
#include "UI/Base/DSWidgetLayer.h"


UDSUIManagerSubsystem::UDSUIManagerSubsystem()
	: Super()
	, PrimaryWidget(nullptr)
{
	
}

UDSUIManagerSubsystem* UDSUIManagerSubsystem::Get(UObject* Object)
{
	UWorld* World = Object->GetWorld();

	check(World);

	UDSGameInstance* GameInstance = Cast<UDSGameInstance>(World->GetGameInstance());

	check(GameInstance);
	return GameInstance->GetSubsystem<UDSUIManagerSubsystem>();
}



UUserWidget* UDSUIManagerSubsystem::PushContentToLayer(FGameplayTag LayerName)
{
	if (IsValid(PrimaryWidget))
	{
		UUserWidget* NewWidget = PrimaryWidget->PushContentToLayer(LayerName);

		if (IsValid(NewWidget))
		{
			return NewWidget;
		}
	}	
	
	return nullptr;
}

void UDSUIManagerSubsystem::PopContentToLayer(FGameplayTag LayerName)
{
	if (IsValid(PrimaryWidget))
	{
		PrimaryWidget->PopContentfromLayer(LayerName);
	}
}

void UDSUIManagerSubsystem::ClearLayer(FGameplayTag LayerName)
{
	if (IsValid(PrimaryWidget))
	{
		PrimaryWidget->ClearLayer(LayerName);
	}
}

void UDSUIManagerSubsystem::FocusGame(APlayerController* PlayerController)
{
	if (IsValid(PlayerController))
	{
		ADSPlayerController* DSPlayerController = Cast<ADSPlayerController>(PlayerController);

		if (IsValid(DSPlayerController))
		{
			DSPlayerController->SetGameFocusMode();
		}
	}	
}

void UDSUIManagerSubsystem::FocusModal(APlayerController* PlayerController)
{
	if (IsValid(PlayerController))
	{
		ADSPlayerController* DSPlayerController = Cast<ADSPlayerController>(PlayerController);

		if (IsValid(DSPlayerController))
		{
			DSPlayerController->SetUIFocusMode();
		}
	}
}

void UDSUIManagerSubsystem::SetMovementEnable(ACharacter* Character, bool bEnable)
{
	if (IsValid(Character))
	{
		if (bEnable)
		{
			if (IsValid(Character->GetCharacterMovement()))
			{
				Character->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
			}
			
		}
		else
		{
			if (IsValid(Character->GetCharacterMovement()))
			{
				Character->GetCharacterMovement()->DisableMovement();
			}
		}
	}
	
}

bool UDSUIManagerSubsystem::RegisterWidget(UDSPrimaryLayout* Widget)
{
	if (IsValid(Widget))
	{
		//포인터 변경으로 GC에 의해 처리하도록 한다.
		PrimaryWidget = Widget;
		return true;
	}
	return false;
}

UUserWidget* UDSUIManagerSubsystem::GetTopLayer(FGameplayTag WidgetTag)
{
	if (IsValid(PrimaryWidget))
	{
		UDSWidgetLayer* DSWidgetLayer = PrimaryWidget->FindLayerByTag(WidgetTag);

		if (IsValid(DSWidgetLayer))
		{
			return DSWidgetLayer->GetTopWidget();
		}
	}
	return nullptr;
}

