// Default
#include "Components/DSInventoryComponent.h"

// UE
#include "EngineUtils.h"
#include "GameFramework/PlayerState.h"

// Game
#include "Character/Characters/DSCharacter.h"
#include "DSLogChannels.h"
#include "GameData/DSEnums.h"
#include "GameData/Items/DSItemData.h"
#include "Item/DSItem.h"
#include "Item/DSItemAccessory.h"
#include "Item/DSItemActor.h"
#include "Item/DSItemGrenade.h"
#include "Item/DSItemPotion.h"
#include "Item/DSItemVehicle.h"
#include "Player/DSPlayerController.h"
#include "Player/DSPlayerState.h"
#include "System/DSEventSystems.h"
#include "System/DSGameDataSubsystem.h"

UDSInventoryComponent::UDSInventoryComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}



EInventoryCategory UDSInventoryComponent::GetCategoryByItemID(int32 ItemID)
{
	UDSGameDataSubsystem* DataSubsystem = UDSGameDataSubsystem::Get(this);
	check(DataSubsystem);
	EItemType ItemType = IDSItem::ConvertToItemType(ItemID);
	return (ItemType == EItemType::Vehicle) ? EInventoryCategory::PersonalVehicle : EInventoryCategory::PersonalItem;
}

FIntPoint UDSInventoryComponent::IndexToTile(int32 Index, int32 Columns)
{
	FIntPoint Tile;

	Tile.X = Index % Columns;
	Tile.Y = Index / Columns;

	return Tile;

}

int32 UDSInventoryComponent::TiletoIndex(FIntPoint DSTile, int32 Columns)
{
	return DSTile.X + DSTile.Y * Columns;
}

void UDSInventoryComponent::OnRegister()
{
	Super::OnRegister();

	// 개인 인벤토리
	InventoryArrayMap.Add(EInventoryCategory::PersonalItem, TArray<FDSItemInfo>());
	InventoryInfoMap.Add(EInventoryCategory::PersonalItem, FInventoryInfo(6, 6, 70));

	// 개인 탈 것 인벤토리
	InventoryArrayMap.Add(EInventoryCategory::PersonalVehicle, TArray<FDSItemInfo>());
	InventoryInfoMap.Add(EInventoryCategory::PersonalVehicle, FInventoryInfo(3, 3, 70 ));

	// 배열 초기화
	for (auto& Pair : InventoryInfoMap)
	{
		int32 SlotCount = Pair.Value.Columns * Pair.Value.Rows;
		InventoryArrayMap[Pair.Key].SetNum(SlotCount);

		for (FDSItemInfo& Slot : InventoryArrayMap[Pair.Key])
		{
			Slot.ID = 0;
			Slot.isRotated = false;
		}
	}
}

bool UDSInventoryComponent::StoreItems(ADSItemActor* ItemActor, const FDSItemInfo& ItemDataInfo)
{

	FIntPoint StoredTopLeft;
	bool bIsItemStored = TryAddItem(ItemDataInfo, &StoredTopLeft);

	// 아이템이 저장됨 
	if (bIsItemStored)
	{
		// 🔹 리슨 서버에서는 별도의 RPC 호출 없이 로컬에서 처리
		if (GetOwner()->HasAuthority())
		{
			if (IsValid(ItemActor))
			{
				ItemActor->SetLifeSpan(0.3f);
			}
			DSEVENT_DELEGATE_INVOKE(OnItemAcquired, ItemDataInfo.ID, false);
			return bIsItemStored;
		}
		DSEVENT_DELEGATE_INVOKE(OnItemAcquired, ItemDataInfo.ID, false);
		ServerRPC_StoreItems(ItemActor, ItemDataInfo, StoredTopLeft);

		return true;
	}

	DSEVENT_DELEGATE_INVOKE(OnInventoryFull, EAlertStatus::InventoryFull);
	return false;
}

void UDSInventoryComponent::UseItem(const FDSItemInfo& ItemInfo, FIntPoint TopLeftIndex)
{

	if (GetNetMode() != ENetMode::NM_ListenServer)
	{
		ExecuteRemoveItem(ItemInfo, TopLeftIndex);
	}
	ServerRPC_UseItem(ItemInfo, TopLeftIndex);

	EInventoryCategory InventoryCategory = GetCategoryByItemID(ItemInfo.ID);
	DSEVENT_DELEGATE_INVOKE(OnInventorySlotUpdated[InventoryCategory], ItemInfo, TopLeftIndex, true);
}

void UDSInventoryComponent::OnItemUsed(int32 ItemID)
{

	UDSGameDataSubsystem* DataSubsystem = UDSGameDataSubsystem::Get(this);

	check(DataSubsystem);

	EItemType ItemType = IDSItem::ConvertToItemType(ItemID);

	//ConvertToItemType에 의해
	FTableRowBase* ItemData = nullptr;

	IDSItem* SelectedItem = nullptr;

	//초기화 리스트를 사용해서 DataTable에 값을 초기화해준다.
	switch (ItemType)
	{
	case EItemType::Accessory:
		ItemData = DataSubsystem->GetDataRowByID(EDataTableType::ItemAccessoryData, ItemID);
		SelectedItem = NewObject<UDSItemAccessory>();
		break;
	case EItemType::Grenade:
		ItemData = DataSubsystem->GetDataRowByID(EDataTableType::ItemGrenadeData, ItemID);
		SelectedItem = NewObject<UDSItemGrenade>();
		break;
	case EItemType::Potion:
		ItemData = DataSubsystem->GetDataRowByID(EDataTableType::ItemPotionData, ItemID);
		SelectedItem = NewObject<UDSItemPotion>();
		break;
	case EItemType::Vehicle:
		ItemData = DataSubsystem->GetDataRowByID(EDataTableType::ItemVehicleData, ItemID);
		SelectedItem = NewObject<UDSItemVehicle>();
		break;
	}

	if (nullptr != SelectedItem)
	{
		//데이터 초기화
		SelectedItem->Initialize(ItemData);

		ADSCharacterBase* Character = Cast<ADSCharacterBase>(GetOwner());

		if (IsValid(Character))
		{
			//실제 Stat 적용 로직
			SelectedItem->UseItem(Character->GetStatComponent());
		}

		DS_LOG(DSItemLog, Warning, TEXT("ItemID is valid"));
	}
	else
	{
		DS_LOG(DSItemLog, Warning, TEXT("ItemID is not valid"));
	}
}

bool UDSInventoryComponent::IsRoomAvailable(const FDSItemInfo& ItemInfo, int32 TopLeftIndex)
{
	const EInventoryCategory Category = GetCategoryByItemID(ItemInfo.ID);
	TArray<FDSItemInfo>* Inventory = nullptr;
	FInventoryInfo* InventoryInfo = nullptr;

	if(false == TryGetInventoryArrayAndInfo(ItemInfo.ID, Inventory, InventoryInfo))
	{
		return false;
	}

	UDSGameDataSubsystem* DataManager = UDSGameDataSubsystem::Get(this);
	check(DataManager);
	FDSItemData* ItemData = static_cast<FDSItemData*>(DataManager->GetDataRowByID(EDataTableType::ItemData, ItemInfo.ID));
	if (nullptr == ItemData)
	{
		return false;
	}
	const FIntPoint Dimensions = ItemData->Dimensions;
	FIntPoint StartTile = IndexToTile(TopLeftIndex, InventoryInfo->Columns);
	const FIntPoint FinalSize = ItemInfo.isRotated ? FIntPoint(Dimensions.Y, Dimensions.X) : Dimensions;

	for (int32 y = 0; y < FinalSize.Y; ++y)
	{
		for (int32 x = 0; x < FinalSize.X; ++x)
		{
			FIntPoint TileToCheck(StartTile.X + x, StartTile.Y + y);


			if (TileToCheck.X < 0 || TileToCheck.X >= InventoryInfo->Columns ||
				TileToCheck.Y < 0 || TileToCheck.Y >= InventoryInfo->Rows)
			{
				return false;
			}
			int32 Index = TiletoIndex(TileToCheck, InventoryInfo->Columns);
			if (IsItemAtIndex(Index, Category))
			{
				return false;
			}
		}
	}

	return true;
}

void UDSInventoryComponent::AddItemAt(const FDSItemInfo& ItemInfo, int32 TopLeftIndex, bool bNotifyServer)
{
	TArray<FDSItemInfo>* Inventory = nullptr;
	FInventoryInfo* InventoryInfo = nullptr;

	if(false == TryGetInventoryArrayAndInfo(ItemInfo.ID, Inventory, InventoryInfo))
	{
		return ;
	}

	UDSGameDataSubsystem* DataManager = UDSGameDataSubsystem::Get(this);
	check(DataManager);
	FDSItemData* ItemData = static_cast<FDSItemData*>(DataManager->GetDataRowByID(EDataTableType::ItemData, ItemInfo.ID));
	FIntPoint Dimensions = ItemData->Dimensions;
	FIntPoint StartTile = IndexToTile(TopLeftIndex, InventoryInfo->Columns);
	const FIntPoint FinalSize = ItemInfo.isRotated ? FIntPoint(Dimensions.Y, Dimensions.X) : Dimensions;

	for (int32 y = 0; y < FinalSize.Y; ++y)
	{
		for (int32 x = 0; x < FinalSize.X; ++x)
		{
			const FIntPoint Tile = StartTile + FIntPoint(x, y);
			const int32 Index = TiletoIndex(Tile, InventoryInfo->Columns);

			if (Inventory->IsValidIndex(Index))
			{
				FDSItemInfo& Slot = (*Inventory)[Index];
				Slot.ID = ItemInfo.ID;
				Slot.isRotated = ItemInfo.isRotated;
			}
		}
	}


}


bool UDSInventoryComponent::TryAddItem(const FDSItemInfo& ItemDataInfo, FIntPoint* OutTopLeft)
{
	const EInventoryCategory Category = GetCategoryByItemID(ItemDataInfo.ID);
	TArray<FDSItemInfo>* Inventory = nullptr;
	FInventoryInfo* InventoryInfo = nullptr;

	if(false == TryGetInventoryArrayAndInfo(ItemDataInfo.ID, Inventory, InventoryInfo))
	{
		return false;
	}

	const int32 TotalSlots = InventoryInfo->Columns * InventoryInfo->Rows;
	for (int32 Index = 0; Index < TotalSlots; ++Index)
	{
		if (IsRoomAvailable(ItemDataInfo, Index))
		{
			AddItemAt(ItemDataInfo, Index, false);

			*OutTopLeft = IndexToTile(Index, InventoryInfo->Columns);

			if(OnInventorySlotUpdated.Contains(Category))
			{
				DSEVENT_DELEGATE_INVOKE(OnInventorySlotUpdated[Category], ItemDataInfo, *OutTopLeft, false);
				return true;
			}
		}
	}

	return false;
}

void UDSInventoryComponent::RemoveItem(const FDSItemInfo& ItemInfo, FIntPoint TopLeftIndex)
{
	if (GetNetMode() != ENetMode::NM_ListenServer)
	{
		ExecuteRemoveItem(ItemInfo, TopLeftIndex);
	}

	ServerRPC_RemoveItem(ItemInfo, TopLeftIndex);

	const EInventoryCategory InventoryCategory = GetCategoryByItemID(ItemInfo.ID);

	DSEVENT_DELEGATE_INVOKE(OnInventorySlotUpdated[InventoryCategory], ItemInfo, TopLeftIndex, false);
}

void UDSInventoryComponent::AddItemAtFromGrid(const FDSItemInfo& ItemInfo, int32 TopLeftIndex)
{
	const EInventoryCategory Category = GetCategoryByItemID(ItemInfo.ID);
	TArray<FDSItemInfo>* Inventory = nullptr;
	FInventoryInfo* InventoryInfo = nullptr;

	if(false == TryGetInventoryArrayAndInfo(ItemInfo.ID, Inventory, InventoryInfo))
	{
		return ;
	}

	if (GetNetMode() != ENetMode::NM_ListenServer)
	{
		AddItemAt(ItemInfo, TopLeftIndex, true);
	}
	FIntPoint TopLeftTile = IndexToTile(TopLeftIndex, InventoryInfo->Columns);
	ServerRPC_StoreItems(nullptr, ItemInfo, TopLeftTile);
	DSEVENT_DELEGATE_INVOKE(OnInventorySlotUpdated[Category], ItemInfo, TopLeftTile, false);
}

bool UDSInventoryComponent::StoreReceivedItem(ADSItemActor* ItemActor, const FDSItemInfo& ItemDataInfo)
{
	// Server 만 들어옴
	FIntPoint StoredTopLeft;
	bool bIsItemStored = TryAddItem(ItemDataInfo, &StoredTopLeft);

	// 아이템이 저장됨 
	if (bIsItemStored)
	{
		APawn* OwnerPawn = Cast<APawn>(GetOwner());
		if (false == IsValid(OwnerPawn))
		{
			return false;
		}
		if (false == OwnerPawn->IsLocallyControlled())
		{
			ClientRPC_StoreItems(ItemDataInfo, StoredTopLeft, false);
		}
		else
		{
			DSEVENT_DELEGATE_INVOKE(OnItemAcquired, ItemDataInfo.ID, false);
		}
		// 전송 성공 알람
		//DSEVENT_DELEGATE_INVOKE(OnInventoryFull, EAlertStatus::InventoryFull);
		return true;
	}
	//else
	{
		// 전송 실패 알람
		//DSEVENT_DELEGATE_INVOKE(OnInventoryFull, EAlertStatus::InventoryFull);
		return false;
	}

	//return false;
}

void UDSInventoryComponent::RestoreItem(const FDSItemInfo& ItemInfo, FIntPoint TopLeftIndex)
{
	const EInventoryCategory Category = GetCategoryByItemID(ItemInfo.ID);
	TArray<FDSItemInfo>* Inventory = nullptr;
	FInventoryInfo* InventoryInfo = nullptr;

	if(false == TryGetInventoryArrayAndInfo(ItemInfo.ID, Inventory, InventoryInfo))
	{
		return ;
	}

	UDSGameDataSubsystem* Data = UDSGameDataSubsystem::Get(this);
	check(Data);
	FDSItemData* ItemData = static_cast<FDSItemData*>(Data->GetDataRowByID(EDataTableType::ItemData, ItemInfo.ID));
	if (nullptr == ItemData)
	{
		return;
	}
	const FIntPoint Dimensions = ItemData->Dimensions;
	const FIntPoint FinalSize = ItemInfo.isRotated ? FIntPoint(Dimensions.Y, Dimensions.X) : Dimensions;

	for (int32 y = 0; y < FinalSize.Y; ++y)
	{
		for (int32 x = 0; x < FinalSize.X; ++x)
		{
			const FIntPoint Tile = TopLeftIndex + FIntPoint(x, y);
			const int32 Index = TiletoIndex(Tile, InventoryInfo->Columns);

			if (Inventory->IsValidIndex(Index))
			{
				FDSItemInfo& Slot = (*Inventory)[Index];
				Slot.ID = ItemInfo.ID;
				Slot.isRotated = ItemInfo.isRotated;
			}
		}
	}

	DSEVENT_DELEGATE_INVOKE(OnInventorySlotUpdated[Category], ItemInfo, TopLeftIndex, false);
}

void UDSInventoryComponent::RestoreItem(const FDSItemInfo& ItemInfo)
{
	// Server만 들어온다
	FIntPoint StoredTopLeft;
	bool bIsItemStored = TryAddItem(ItemInfo, &StoredTopLeft);

	// 아이템이 저장됨 
	if (bIsItemStored && false == GetOwner()->HasAuthority())
	{
		ClientRPC_StoreItems(ItemInfo, StoredTopLeft, true);
	}
}

bool UDSInventoryComponent::IsItemAtIndex(int32 Index, EInventoryCategory InventoryCategory)
{
	TArray<FDSItemInfo>* Inventory = InventoryArrayMap.Find(InventoryCategory);
	if (nullptr == Inventory)
	{
		return false;
	}
	if (false == Inventory->IsValidIndex(Index))
	{
		return false;
	}

	return (*Inventory)[Index].ID > 0;

}

void UDSInventoryComponent::ExecuteRemoveItem(const FDSItemInfo& ItemInfo, FIntPoint TopLeftIndex)
{
	TArray<FDSItemInfo>* Inventory = nullptr;
	FInventoryInfo* InventoryInfo = nullptr;

	if(false == TryGetInventoryArrayAndInfo(ItemInfo.ID, Inventory, InventoryInfo))
	{
		return ;
	}

	UDSGameDataSubsystem* DataManager = UDSGameDataSubsystem::Get(this);
	check(DataManager);
	FDSItemData* ItemData = static_cast<FDSItemData*>(DataManager->GetDataRowByID(EDataTableType::ItemData, ItemInfo.ID));
	if (nullptr == ItemData)
	{
		return;
	}

	const FIntPoint Dimensions = ItemData->Dimensions;
	const FIntPoint FinalSize = ItemInfo.isRotated ? FIntPoint(Dimensions.Y, Dimensions.X) : Dimensions;

	for (int32 y = 0; y < FinalSize.Y; ++y)
	{
		for (int32 x = 0; x < FinalSize.X; ++x)
		{
			const FIntPoint Tile = TopLeftIndex + FIntPoint(x, y);
			const int32 Index = TiletoIndex(Tile, InventoryInfo->Columns);

			if (Inventory->IsValidIndex(Index))
			{
				FDSItemInfo& Slot = (*Inventory)[Index];
				Slot.ID = 0;
				Slot.isRotated = false;
			}
		}
	}
}

void UDSInventoryComponent::ExecuteItemUse(const FDSItemInfo& ItemInfo, FIntPoint TopLeftIndex)
{
	UWorld* World = GetWorld();
	check(World);

	// 아이템 사용
	OnItemUsed(ItemInfo.ID);

	// 서버 인벤토리에서 삭제
	ExecuteRemoveItem(ItemInfo, TopLeftIndex);

	// ClientRPC_RemoveItem(ItemInfo, TopLeftIndex);

	APlayerController* LocalPlayertController = GetController<APlayerController>();

	if (IsValid(LocalPlayertController))
	{
		//모든 클라이언트를 가지고와서 이펙트 사용!!
		for (APlayerController* PlayerController : TActorRange<APlayerController>(GetWorld()))
		{
			if (IsValid(PlayerController) && LocalPlayertController != PlayerController)
			{
				if (!PlayerController->IsLocalController())
				{
					ADSCharacter* OtherPlayer = Cast<ADSCharacter>(PlayerController->GetPawn());
					if (OtherPlayer)
					{
						//이펙트 실행
					}
				}
			}
		}
	}
}

bool UDSInventoryComponent::ServerRPC_StoreItems_Validate(ADSItemActor* ItemActor, const FDSItemInfo& ItemInfo, FIntPoint TopLeftIndex)
{
	TArray<FDSItemInfo>* Inventory = nullptr;
	FInventoryInfo* InventoryInfo = nullptr;

	if(false == TryGetInventoryArrayAndInfo(ItemInfo.ID, Inventory, InventoryInfo))
	{
		return false;
	}

	UDSGameDataSubsystem* Data = UDSGameDataSubsystem::Get(this);
	check(Data);
	FDSItemData* ItemData = static_cast<FDSItemData*>(Data->GetDataRowByID(EDataTableType::ItemData, ItemInfo.ID));
	if (nullptr == ItemData)
	{
		return false;
	}

	const FIntPoint FinalSize = ItemInfo.isRotated ? FIntPoint(ItemData->Dimensions.Y, ItemData->Dimensions.X) : ItemData->Dimensions;

	// 아이템 크기 만큼 모든 슬롯을 검사
	for (int32 y = 0; y < FinalSize.Y; ++y)
	{
		for (int32 x = 0; x < FinalSize.X; ++x)
		{
			FIntPoint Tile = TopLeftIndex + FIntPoint(x, y);
			int32 CheckIdx = TiletoIndex(Tile, InventoryInfo->Columns);

			if (!Inventory->IsValidIndex(CheckIdx)) return false;

			const FDSItemInfo& CheckSlot = (*Inventory)[CheckIdx];
			if (CheckSlot.ID != 0 || CheckSlot.isRotated != false)
			{
				// 다른 아이템이 있으면 롤백
				ClientRPC_RollbackItems(true, ItemInfo, TopLeftIndex);
				return false;
			}
		}
	}
	return true;
}

void UDSInventoryComponent::ServerRPC_StoreItems_Implementation(ADSItemActor* ItemActor, const FDSItemInfo& ItemInfo, FIntPoint TopLeftIndex)
{
	// 전용 서버에서도 아이템을 저장하도록 수정
	TArray<FDSItemInfo>* Inventory = nullptr;
	FInventoryInfo* InventoryInfo = nullptr;

	if(false == TryGetInventoryArrayAndInfo(ItemInfo.ID, Inventory, InventoryInfo))
	{
		return;
	}

	AddItemAt(ItemInfo, TiletoIndex(TopLeftIndex, InventoryInfo->Columns), false);

	if (IsValid(ItemActor))
	{
		ItemActor->SetLifeSpan(0.3f);
	}
}

bool UDSInventoryComponent::ServerRPC_UseItem_Validate(const FDSItemInfo& ItemInfo, FIntPoint TopLeftIndex)
{
	TArray<FDSItemInfo>* Inventory = nullptr;
	FInventoryInfo* InventoryInfo = nullptr;

	if(false == TryGetInventoryArrayAndInfo(ItemInfo.ID, Inventory, InventoryInfo))
	{
		return false;
	}

	// 전체 타일 검증
	UDSGameDataSubsystem* Data = UDSGameDataSubsystem::Get(this);
	check(Data);
	FDSItemData* ItemData = static_cast<FDSItemData*>(Data->GetDataRowByID(EDataTableType::ItemData, ItemInfo.ID));
	if (nullptr == ItemData)
	{
		return false;
	}

	const FIntPoint FinalSize = ItemInfo.isRotated ? FIntPoint(ItemData->Dimensions.Y, ItemData->Dimensions.X) : ItemData->Dimensions;

	for (int32 y = 0; y < FinalSize.Y; ++y)
	{
		for (int32 x = 0; x < FinalSize.X; ++x)
		{
			FIntPoint Tile = TopLeftIndex + FIntPoint(x, y);
			int32 CheckIdx = TiletoIndex(Tile, InventoryInfo->Columns);

			if (false == Inventory->IsValidIndex(CheckIdx))
			{
				return false;
			}

			const FDSItemInfo& CheckSlot = (*Inventory)[CheckIdx];
			if (CheckSlot.ID != ItemInfo.ID || CheckSlot.isRotated != ItemInfo.isRotated)
			{
				ClientRPC_RollbackItems(true, ItemInfo, TopLeftIndex);
				return false;
			}
		}
	}

	return true;
}

void UDSInventoryComponent::ServerRPC_UseItem_Implementation(const FDSItemInfo& ItemInfo, FIntPoint TopLeftIndex)
{
	ExecuteItemUse(ItemInfo, TopLeftIndex);
}

bool UDSInventoryComponent::ServerRPC_RemoveItem_Validate(const FDSItemInfo& ItemInfo, FIntPoint TopLeftIndex)
{
	TArray<FDSItemInfo>* Inventory = nullptr;
	FInventoryInfo* InventoryInfo = nullptr;

	if(false == TryGetInventoryArrayAndInfo(ItemInfo.ID, Inventory, InventoryInfo))
	{
		return false;
	}

	// 전체 타일 검증
	UDSGameDataSubsystem* Data = UDSGameDataSubsystem::Get(this);
	check(Data);
	FDSItemData* ItemData = static_cast<FDSItemData*>(Data->GetDataRowByID(EDataTableType::ItemData, ItemInfo.ID));
	if (nullptr == ItemData)
	{
		return false;
	}

	const FIntPoint FinalSize = ItemInfo.isRotated ? FIntPoint(ItemData->Dimensions.Y, ItemData->Dimensions.X) : ItemData->Dimensions;

	for (int32 y = 0; y < FinalSize.Y; ++y)
	{
		for (int32 x = 0; x < FinalSize.X; ++x)
		{
			FIntPoint Tile = TopLeftIndex + FIntPoint(x, y);
			int32 CheckIdx = TiletoIndex(Tile, InventoryInfo->Columns);

			if (false == Inventory->IsValidIndex(CheckIdx))
			{
				return false;
			}

			const FDSItemInfo& CheckSlot = (*Inventory)[CheckIdx];
			if (CheckSlot.ID != ItemInfo.ID || CheckSlot.isRotated != ItemInfo.isRotated)
			{
				ClientRPC_RollbackItems(true, ItemInfo, TopLeftIndex);
				return false;
			}
		}
	}

	return true;
}

void UDSInventoryComponent::ServerRPC_RemoveItem_Implementation(const FDSItemInfo& ItemInfo, FIntPoint TopLeftIndex)
{
	// 서버 인벤토리에서 삭제
	ExecuteRemoveItem(ItemInfo, TopLeftIndex);
}

void UDSInventoryComponent::ClientRPC_StoreItems_Implementation(const FDSItemInfo& ItemInfo, FIntPoint TopLeftIndex, bool bRestore)
{
	const EInventoryCategory Category = GetCategoryByItemID(ItemInfo.ID);
	TArray<FDSItemInfo>* Inventory = nullptr;
	FInventoryInfo* InventoryInfo = nullptr;

	if(false == TryGetInventoryArrayAndInfo(ItemInfo.ID, Inventory, InventoryInfo))
	{
		return;
	}

	AddItemAt(ItemInfo, TiletoIndex(TopLeftIndex, InventoryInfo->Columns), false);
	if (false == bRestore)
	{
		DSEVENT_DELEGATE_INVOKE(OnItemAcquired, ItemInfo.ID, false);
	}

	DSEVENT_DELEGATE_INVOKE(OnInventorySlotUpdated[Category], ItemInfo, TopLeftIndex, true);
}

void UDSInventoryComponent::ClientRPC_RemoveItem_Implementation(const FDSItemInfo& ItemInfo, FIntPoint TopLeftIndex)
{
	ExecuteRemoveItem(ItemInfo, TopLeftIndex);
	EInventoryCategory InventoryCategory = GetCategoryByItemID(ItemInfo.ID);
	
	DSEVENT_DELEGATE_INVOKE(OnInventorySlotUpdated[InventoryCategory], ItemInfo, TopLeftIndex, true);
}

void UDSInventoryComponent::ClientRPC_RollbackItems_Implementation(bool bIsPlus, const FDSItemInfo& ItemData, FIntPoint TopLeftIndex)
{
	if (false == bIsPlus)
	{
		RestoreItem(ItemData, TopLeftIndex);
	}
	else
	{
		RemoveItem(ItemData, TopLeftIndex);
	}
}

bool UDSInventoryComponent::TryGetInventoryArrayAndInfo(int32 ItemID, TArray<FDSItemInfo>*& Inventory, FInventoryInfo*& InventoryInfo)
{
	EInventoryCategory Category = GetCategoryByItemID(ItemID);
	Inventory = InventoryArrayMap.Find(Category);
	InventoryInfo = InventoryInfoMap.Find(Category);

	if (nullptr == Inventory || nullptr == InventoryInfo)
	{
		return false;
	}

	return true;
}

void UDSInventoryComponent::PrintItem()
{
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* PlayerController = Iterator->Get();
		if (!PlayerController) continue;

		// PlayerState 로그 출력
		APlayerState* PlayerState = PlayerController->PlayerState;
		if (!PlayerState) continue;

		FString PlayerID = FString::Printf(TEXT("Player ID: %d"), PlayerState->GetPlayerId());
		UE_LOG(LogTemp, Log, TEXT("%s"), *PlayerID);

		// 컨트롤러가 조종 중인 Pawn 얻기
		APawn* Pawn = PlayerController->GetPawn();
		if (!Pawn) continue;

		// 인벤토리 컴포넌트 가져오기 (여기선 UInventoryComponent가 있다고 가정)
		UDSInventoryComponent* InventoryComp = Pawn->FindComponentByClass<UDSInventoryComponent>();
		if (!InventoryComp) continue;

		// 인벤토리 맵과 정보 가져오기
		const TMap<EInventoryCategory, TArray<FDSItemInfo>>& InventoryMapPrint = InventoryComp->InventoryArrayMap;
		const TMap<EInventoryCategory, FInventoryInfo>& InventoryInfoMapPrint = InventoryComp->InventoryInfoMap;

		// 카테고리별 인벤토리 출력
		for (const auto& Pair : InventoryInfoMapPrint)
		{
			EInventoryCategory Category = Pair.Key;
			const FInventoryInfo& Info = Pair.Value;

			const TArray<FDSItemInfo>* Inventory = InventoryMapPrint.Find(Category);
			if (!Inventory) continue;

			FString CategoryInfo = FString::Printf(TEXT("== Category: %s =="), *UEnum::GetValueAsString(Category));
			UE_LOG(LogTemp, Log, TEXT("%s"), *CategoryInfo);

			for (int32 Y = 0; Y < Info.Rows; ++Y)
			{
				FString RowString;

				for (int32 X = 0; X < Info.Columns; ++X)
				{
					int32 Index = Y * Info.Columns + X;

					if (!Inventory->IsValidIndex(Index))
					{
						RowString += TEXT(" ? ");
						continue;
					}

					int32 ID = (*Inventory)[Index].ID;

					if (ID == 0)
					{
						RowString += TEXT(" . ");
					}
					else
					{
						RowString += FString::Printf(TEXT("%d "), ID);
					}
				}

				UE_LOG(LogTemp, Log, TEXT("%s"), *RowString);
			}
		}
	}

}