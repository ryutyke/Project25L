// Default
#include "UI/Game/DSMainWidget.h"

// UE
#include "Components/VerticalBox.h"
#include "Components/Overlay.h"
#include "Components/TextBlock.h"

// Game
#include "Character/Characters/DSCharacter.h"
#include "Components/DSInventoryComponent.h"
#include "System/DSEventSystems.h"
#include "System/DSGameUtils.h"
#include "UI/Game/Player/DSHPBar.h"
#include "UI/Game/Player/DSPlayerInfo.h"
#include "UI/Item/DSItemPickupSlot.h"

#include <Player/DSPlayerController.h>
#include <Kismet/GameplayStatics.h>
#include <Game/DSGameMode.h>
UDSMainWidget::UDSMainWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UDSMainWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (IsValid(PlayerInfo))
	{
		//0번 Idx는 현재 플레이어 값
		PlayerInfos.Add(PlayerInfo);
	}

	if (IsValid(VBox_Info))
	{
		TArray<UWidget*> Widgets = VBox_Info->GetAllChildren();

		for (int WidgetIdx = 0; WidgetIdx < Widgets.Num(); WidgetIdx++)
		{
			UDSPlayerInfo* EntryInfo = Cast<UDSPlayerInfo>(Widgets[WidgetIdx]);

			if (IsValid(EntryInfo))
			{
				PlayerInfos.Add(EntryInfo);
			}
		}
	}

	DSEVENT_DELEGATE_BIND(GameEvent.OnCharacterSpawned, this, &UDSMainWidget::InvalidateWidget);

}


void UDSMainWidget::InvalidateWidget()
{
	ADSCharacter* Character = Cast<ADSCharacter>(UDSGameUtils::GetCharacter(GetOwningPlayer()));

	if (false == IsValid(Character))
	{
		return;
	}

	UDSInventoryComponent* Inventory = Character->GetInventoryComponent();
	if (false == IsValid(Inventory))
	{
		return;
	}

	if (IsValid(ItemPickupSlot))
	{
		DSEVENT_DELEGATE_BIND(Inventory->OnItemAcquired, this, &UDSMainWidget::SetupItemPickupSlot);
		ItemPickupSlot->SetVisibility(ESlateVisibility::Collapsed);
		ItemPickupSlot->InvalidateWidget(Inventory);
	}

	if (IsValid(Overlay_AlertType))
	{
		Overlay_AlertType->SetVisibility(ESlateVisibility::Collapsed);
		DSEVENT_DELEGATE_BIND(Inventory->OnInventoryFull, this, &UDSMainWidget::SetupAlertComment);
	}
}

void UDSMainWidget::SetupItemPickupSlot(int32 ItemID, bool bRemoved)
{
	if (false == bRemoved)
	{
		ItemPickupSlot->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
}

void UDSMainWidget::SetupAlertComment(EAlertStatus AlertStatus)
{
	if (AlertComment.Contains(AlertStatus))
	{
		FText& AlertText = AlertComment[AlertStatus];

		if (IsValid(Text_AlertComment))
		{
			Text_AlertComment->SetText(AlertText);
		}

		if (IsValid(Overlay_AlertType))
		{
			Overlay_AlertType->SetVisibility(ESlateVisibility::Visible);
			FTimerHandle TimerHandleAlert;
			GetWorld()->GetTimerManager().SetTimer(TimerHandleAlert,this, &UDSMainWidget::HideAlertOverlay,1.0f, false);
		}
	}
}

void UDSMainWidget::HideAlertOverlay()
{
	if (IsValid(Overlay_AlertType))
	{
		Overlay_AlertType->SetVisibility(ESlateVisibility::Collapsed);
	}
}

UDSHPBar* UDSMainWidget::GetHPBar(int32 PlayerIndex)
{
	if (PlayerInfos.IsValidIndex(PlayerIndex))
	{
		//인덱스를 포함하고 있다면,

		UDSPlayerInfo* EntryInfo = PlayerInfos[PlayerIndex];

		if (IsValid(PlayerInfo))
		{
			return EntryInfo->GetHPBar();
		}
	}
	return nullptr;
}

UDSPlayerInfo* UDSMainWidget::GetPlayerInfo(int32 PlayerIndex)
{
	if (PlayerInfos.IsValidIndex(PlayerIndex))
	{
		//인덱스를 포함하고 있다면,

		UDSPlayerInfo* EntryInfo = PlayerInfos[PlayerIndex];

		if (IsValid(EntryInfo))
		{
			return EntryInfo;
		}
	}
	return nullptr;
}
