// Default
#include "UI/Item/DSItemPickupSlot.h"

// UE
#include "Components/Image.h"
#include "Components/TextBlock.h"

// Game
#include "System/DSGameDataSubsystem.h"
#include "Components/DSInventoryComponent.h"
#include "System/DSEventSystems.h"

UDSItemPickupSlot::UDSItemPickupSlot(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UDSItemPickupSlot::InvalidateWidget(UDSInventoryComponent* Inventory)
{
	if (false == IsValid(Inventory))
	{
		return;
	}

	DSEVENT_DELEGATE_BIND(Inventory->OnItemAcquired, this, &UDSItemPickupSlot::UpdateWidget);
}

void UDSItemPickupSlot::UpdateWidget(int32 ItemID, bool bRemoved)
{
	UDSGameDataSubsystem * DataManager = UDSGameDataSubsystem::Get(this);

	check(DataManager);

	FDSItemData* ItemData = static_cast<FDSItemData*>(DataManager->GetDataRowByID(EDataTableType::ItemData, ItemID));

	if (nullptr != ItemData)
	{
		if (IsValid(IMG_ItemIcon))
		{
			IMG_ItemIcon->SetBrushFromSoftTexture(ItemData->ItemLogo);
		}

		if (IsValid(Text_Alert))
		{
			FString FormattedText = FString::Printf(TEXT("%s을(를) 획득했습니다!"), *ItemData->ItemName.ToString());

			Text_Alert->SetText(FText::FromString(FormattedText));

			UWorld* World = GetWorld();
			check(World);

			World->GetTimerManager().ClearTimer(ItemSlotExpireHandle);
			World->GetTimerManager().SetTimer(ItemSlotExpireHandle, this, &UDSItemPickupSlot::DisappearSlot, 3.0f);
		}
	}
}

void UDSItemPickupSlot::DisappearSlot()
{
	SetVisibility(ESlateVisibility::Collapsed);
}