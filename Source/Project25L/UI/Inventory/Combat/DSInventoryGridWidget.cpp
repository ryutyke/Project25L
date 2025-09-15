// Default
#include "UI/Inventory/Combat/DSInventoryGridWidget.h"

// UE
#include "Components/CanvasPanelSlot.h"
#include "Components/CanvasPanel.h"

// Game
#include "UI/Inventory/DSItemDragDropOperation.h"
#include "Components/DSInventoryComponent.h"
#include "System/DSSpawnerSubsystem.h"
#include "System/DSEventSystems.h"
#include "UI/Inventory/Widget/DSCombatInventoryWidget.h"
#include "UI/Inventory/DSInventoryItem.h"


UDSInventoryGridWidget::UDSInventoryGridWidget(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
}

bool UDSInventoryGridWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{

	const UDSItemDragDropOperation* ItemDragDrop = Cast<UDSItemDragDropOperation>(InOperation);
	if (false == IsValid(ItemDragDrop))
	{
		return false;
	}

	// 다른 카테고리 인벤토리에 넣으면 자기 카테고리로 다시 들어오게
	FIntPoint StoredTopLeft;
	EInventoryCategory DroppedCategory = ItemDragDrop->InventoryCategory;
	const bool bAcceptable = (InventoryCategory == DroppedCategory);
	if (false == bAcceptable)
	{
		InventoryComponent->TryAddItem(ItemDragDrop->ItemInfo, &StoredTopLeft);
		return true;
	}

	bool bSuperHandled = Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

	if (bSuperHandled == true)
	{
		return true;
	}

	if (false == InventoryComponent->TryAddItem(ItemDragDrop->ItemInfo, &StoredTopLeft))
	{
		UDSSpawnerSubsystem* SpawnManager = UDSSpawnerSubsystem::Get(this);
		if (false == IsValid(SpawnManager))
		{
			return false;
		}

		APlayerController* PC = GetWorld()->GetFirstPlayerController();
		if (false == IsValid(PC))
		{
			return false;
		}
		APawn* PlayerPawn = PC->GetPawn();
		if (false == IsValid(PlayerPawn))
		{
			return false;
		}

		FVector2D Offset2D = FMath::RandPointInCircle(150.f);
		FVector RandomLocation = PlayerPawn->GetActorLocation() + FVector(Offset2D.X, Offset2D.Y, 0.f);
		SpawnManager->CreateActor(ESpawnerType::FixedItem, ItemDragDrop->ItemInfo.ID, RandomLocation);
		return true;
	}
	return true;
}

UDSInventoryItemBase* UDSInventoryGridWidget::AddItemWidgetToSlot(const FDSItemInfo& ItemInfo, FIntPoint TopLeftIndex)
{
	UDSInventoryItemBase* ItemWidgetBase = Super::AddItemWidgetToSlot(ItemInfo, TopLeftIndex);
	if(false == IsValid(ItemWidgetBase))
	{
		return nullptr;
	}

	UDSInventoryItem* ItemWidget = Cast<UDSInventoryItem>(ItemWidgetBase);
	if(false == IsValid(ItemWidget))
	{
		return nullptr;
	}

	DSEVENT_DELEGATE_BIND(ItemWidget->OnItemUse, InventoryComponent, &UDSInventoryComponent::UseItem);
	
	UDSCombatInventoryWidget* InventoryWidget = Cast<UDSCombatInventoryWidget>(GetTypedOuter<UDSCombatInventoryWidget>());

	if (IsValid(InventoryWidget))
	{
		DSEVENT_DELEGATE_BIND(ItemWidget->OnItemTooltipRequested, InventoryWidget, &UDSCombatInventoryWidget::SetItemToolTip);
	}

	return ItemWidget;
}
