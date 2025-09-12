// Default
#include "UI/Inventory/Widget/DSCombatInventoryWidget.h"

// UE
#include "Kismet/GameplayStatics.h"
#include "components/CanvasPanelSlot.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"

// Game
#include "Character/Characters/DSCharacter.h"
#include "Components/DSInventoryComponent.h"
#include "system/DSGameUtils.h"
#include "system/DSSpawnerSubsystem.h"
#include "UI/Inventory/Combat/DSInventoryGridWidget.h"
#include "UI/Inventory/DSItemDragDropOperation.h"
#include "Item/DSItemActor.h"
#include "DSLogChannels.h"
#include "UI/Item/DSItemTooltip.h"
#include "System/DSGameDataSubsystem.h"
#include "UI/Inventory/Combat/DSPlayerSendPanel.h"
#include "System/DSEventSystems.h"



void UDSCombatInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();
	// DSEVENT_DELEGATE_BIND(GameEvent.OnCharacterSpawned, this, &UDSCombatInventoryWidget::InitInventoryWidget);
}



bool UDSCombatInventoryWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	bool bSuperHandled = Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
	const UDSItemDragDropOperation* ItemDragDrop = Cast<UDSItemDragDropOperation>(InOperation);
	if (false == IsValid(ItemDragDrop))
	{
		return bSuperHandled;
	}

	UDSSpawnerSubsystem* SpawnManager = UDSSpawnerSubsystem::Get(this);
	if (false == IsValid(SpawnManager))
	{
		return bSuperHandled;
	}

	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (false == IsValid(PC))
	{
		return bSuperHandled;
	}

	APawn* PlayerPawn = PC->GetPawn();
	if (false == IsValid(PlayerPawn))
	{
		return bSuperHandled;
	}

	FVector2D Offset2D = FMath::RandPointInCircle(150.f);
	FVector RandomLocation = PlayerPawn->GetActorLocation() + FVector(Offset2D.X, Offset2D.Y, 0.f);
	TWeakObjectPtr<AActor> Actor = SpawnManager->CreateActor(ESpawnerType::FixedItem, ItemDragDrop->ItemInfo.ID, RandomLocation);
	if (Actor.IsValid())
	{
		ADSItemActor* ItemActor = Cast<ADSItemActor>(Actor);

		if (IsValid(ItemActor))
		{
			TMap<int32, int32> StoredItem;
			StoredItem.Add({ ItemDragDrop->ItemInfo.ID, 1 });
			ItemActor->InitializeItemData(StoredItem);
		}
	}
	return true;
}




void UDSCombatInventoryWidget::SetItemToolTip(const FDSItemInfo& ItemInfo,FIntPoint TopLeftIndex,EInventoryCategory Category,int32 TileSize,bool bIsVisible)
{
	if (false == IsValid(ItemTooltips)) 
	{
		return;
	}

	UDSGameDataSubsystem* DataManager = UDSGameDataSubsystem::Get(this);
	check(DataManager);
	FDSItemData* ItemData = static_cast<FDSItemData*>(DataManager->GetDataRowByID(EDataTableType::ItemData, ItemInfo.ID));
	if (nullptr == ItemData) 
	{
		return;
	}

	UDSInventoryGridWidget* GridWidget = nullptr;
	switch (Category)
	{
	case EInventoryCategory::PersonalItem:
		GridWidget = PersonalItemInventoryWidget;
		break;
	case EInventoryCategory::PersonalVehicle:
		GridWidget = PersonalVehicleInventoryWidget;
		break;
	default:
		break;
	}
	if (false == IsValid(GridWidget)) 
	{
		return;
	}

	const int32 ItemWidthTiles = ItemData->Dimensions.X;
	FVector2D SlotLocalOffset((TopLeftIndex.X + ItemWidthTiles) * TileSize, TopLeftIndex.Y * TileSize);

	// 그리드(local) → 화면절대(absolute)
	const FGeometry& GridGeom = GridWidget->GetCachedGeometry();
	FVector2D AbsoluteSlotPos = GridGeom.LocalToAbsolute(SlotLocalOffset);

	// 화면절대 → 캔버스(local)
	const FGeometry& CanvasGeom = this->GetCachedGeometry();
	FVector2D TooltipLocalPos = CanvasGeom.AbsoluteToLocal(AbsoluteSlotPos + FVector2D(10, 0));

	if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(ItemTooltips->Slot))
	{
		CanvasSlot->SetPosition(TooltipLocalPos);
	}

	ItemTooltips->SetText(ItemInfo.ID);
	ItemTooltips->SetVisibility(bIsVisible ? ESlateVisibility::HitTestInvisible: ESlateVisibility::Collapsed);
}

void UDSCombatInventoryWidget::SetPlayerIdsForSendWidgets()
{
	if (false == IsValid(SendItemWidget0) || false ==  IsValid(SendItemWidget1))
	{
		return;
	}

	UWorld* World = GetWorld();
	check(World);
	AGameStateBase* GameState = World->GetGameState();

	int32 PlayerIndex = 0;
	APlayerState* CurrentPlayerState = GetOwningPlayerState();

	if(false == IsValid(CurrentPlayerState))
	{
		return;
	}

	for (APlayerState* PlayerState : GameState->PlayerArray)
	{
		if (IsValid(PlayerState))
		{
			if (PlayerState == CurrentPlayerState)
			{
				continue;
			}

			if (PlayerIndex == 0)
			{
				int32 PlayerID = PlayerState->GetPlayerId();
				SendItemWidget0->SetPlayerId(PlayerID);
			}
			else if (PlayerIndex == 1)
			{
				int32 PlayerID = PlayerState->GetPlayerId();
				SendItemWidget1->SetPlayerId(PlayerID);
			}
			PlayerIndex++;
		}
	}
}

void UDSCombatInventoryWidget::InitInventoryWidget()
{
	ADSCharacter* Character = Cast<ADSCharacter>(UDSGameUtils::GetCharacter(GetOwningPlayer()));
	if (false == IsValid(Character))
	{
		return;
	}
	UDSInventoryComponent* InventoryComponent = Character->GetInventoryComponent();
	if (false == IsValid(InventoryComponent))
	{
		return;
	}


	if (IsValid(PersonalItemInventoryWidget))
	{
		FInventoryInfo* InventoryInfo = InventoryComponent->GetInventoryInfo(EInventoryCategory::PersonalItem);
		PersonalItemInventoryWidget->Init(EInventoryCategory::PersonalItem, *InventoryInfo);
	}


	if (IsValid(PersonalVehicleInventoryWidget))
	{
		FInventoryInfo* InventoryInfo = InventoryComponent->GetInventoryInfo(EInventoryCategory::PersonalVehicle);
		PersonalVehicleInventoryWidget->Init(EInventoryCategory::PersonalVehicle, *InventoryInfo);
	}

	if (IsValid(ItemTooltips))
	{
		ItemTooltips->SetVisibility(ESlateVisibility::Collapsed);
	}

	SetPlayerIdsForSendWidgets();
}

void UDSCombatInventoryWidget::BindEvents()
{
	PersonalItemInventoryWidget->BindEvents();
	PersonalVehicleInventoryWidget->BindEvents();
}

void UDSCombatInventoryWidget::InitializeWidget()
{
	InitInventoryWidget();
	PersonalItemInventoryWidget->InitializeWidget();
	PersonalVehicleInventoryWidget->InitializeWidget();
}
