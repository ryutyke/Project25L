#pragma once

// Default
#include "CoreMinimal.h"

// UE
#include "Components/PawnComponent.h"

// Game
#include "GameData/Items/DSItemData.h"
#include "GameData/DSEnums.h"
#include "GameData/Items/DSItemData.h"

// UHT
#include "DSInventoryComponent.generated.h"

class IDSItem;
class ADSItemActor;

DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnInventorySlotUpdated, const FDSItemInfo& ItemInfo, FIntPoint TopLeftIndex, bool bRemoved);

UCLASS()
class PROJECT25L_API UDSInventoryComponent : public UPawnComponent
{
	GENERATED_BODY()
	
	public:
	UDSInventoryComponent(const FObjectInitializer& ObjectInitializer);

	//int32, bool 
	DECLARE_MULTICAST_DELEGATE_TwoParams(FOnItemAcquired, int32 ItemID, bool bRemoved);
	FOnItemAcquired OnItemAcquired;

	//int32, int32, bool 
	TMap<EInventoryCategory, FOnInventorySlotUpdated> OnInventorySlotUpdated;
		
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnInventoryFull, EAlertStatus AlertStatus);
	FOnInventoryFull OnInventoryFull;

public:
/*Cheat용*/
	void PrintItem();

public:
	// Helper Functions
	EInventoryCategory GetCategoryByItemID(int32 ItemID);

	static FIntPoint IndexToTile(int32 Index, int32 Columns);
	static int32 TiletoIndex(FIntPoint DSTile, int32 Columns);

	FInventoryInfo* GetInventoryInfo(EInventoryCategory Category) { return InventoryInfoMap.Find(Category); }
public:
	// 배열 크기 등록 & 초기화
	virtual void OnRegister() override;

	/*
	실체가 없는 보물상자와 관련된 아이템을 주울 때에는 ItemID만 필요하다.
	위와 상관없이, 아이템 1개만 줍는 시스템이기 때문에 파라미터를 id를 받아서, ItemActor가 없을 경우 사용하도록 함.
	*/
	bool StoreItems(ADSItemActor* ItemActor, const FDSItemInfo& ItemDataInfo);

	// 키를 사용해서 아이템 사용을 눌렀을 때 실제로 사용 한다.
	// 현재 아이템 타입이 있는지 없는지를 검사를 수행한다.
	void UseItem(const FDSItemInfo& ItemInfo, FIntPoint TopLeftIndex);

	// 아이템 사용시 실제 아이템 스탯이 적용된다.
	void OnItemUsed(int32 ItemID);

	// 인벤토리에서 아이템이 특정 위치(TopLeftIndex)에 들어갈 수 있는지 여부 확인
	bool IsRoomAvailable(const FDSItemInfo& ItemInfo, int32 TopLeftIndex);

	// 주어진 위치에 아이템 배치
	void AddItemAt(const FDSItemInfo& ItemInfo, int32 TopLeftIndex, bool bNotifyServer);

	// 인벤토리에 아이템을 추가할 수 있는지 확인하면서 성공하면 위치를 OutTopLeft에 저장
	bool TryAddItem(const FDSItemInfo& ItemDataInfo, FIntPoint* OutTopLeft);

	// 아이템 삭제 함수
	void RemoveItem(const FDSItemInfo& ItemInfo, FIntPoint TopLeftIndex);

	// 그리드 위젯에서 그 위치에 아이템을 넣고 싶을 때 불리는 함수
	void AddItemAtFromGrid(const FDSItemInfo& ItemInfo, int32 TopLeftIndex);
	
	// 전달받은 아이템 저장하는 함수
	bool StoreReceivedItem(ADSItemActor* ItemActor, const FDSItemInfo& ItemDataInfo);

public:
	// 복구 함수
	// 클라이언트 Rollback에서 불리는 함수
	void RestoreItem(const FDSItemInfo& ItemInfo, FIntPoint TopLeftIndex);

	// 다른 플레이어에게 아이템 전달 실패 후 불리는 함수
	void RestoreItem(const FDSItemInfo& ItemInfo);

protected:
	// 인벤토리 Index 위치에 아이템이 존재하는지 확인
	bool IsItemAtIndex(int32 Index, EInventoryCategory InventoryCategory);

	// 아이템 제거
	void ExecuteRemoveItem(const FDSItemInfo& ItemInfo, FIntPoint TopLeftIndex);

	// 아이템 사용
	void ExecuteItemUse(const FDSItemInfo& ItemInfo, FIntPoint TopLeftIndex);

protected:
	// 저장, 사용, 롤백 RPC

	// 아이템 줍는 함수
	// 서버에게 전달한다.
	// Validate 검사를 수행하지만, return false를 하지 않고, 아이템 위치를 롤백하는 방법을 선택한다.
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRPC_StoreItems(ADSItemActor* ItemActor, const FDSItemInfo& ItemInfo, FIntPoint TopLeftIndex);

	// 아이템 사용 함수
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRPC_UseItem(const FDSItemInfo& ItemInfo, FIntPoint TopLeftIndex);

	// 아이템 삭제 함수
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRPC_RemoveItem(const FDSItemInfo& ItemInfo, FIntPoint TopLeftIndex);

	UFUNCTION(Client, Reliable)
	void ClientRPC_StoreItems(const FDSItemInfo& ItemInfo, FIntPoint TopLeftIndex, bool bRestore);

	
	UFUNCTION(Client, Reliable)
	void ClientRPC_RemoveItem(const FDSItemInfo& ItemInfo, FIntPoint TopLeftIndex);

	// 클라이언트와 서버의 아이템 위치가 다를 경우, 롤백한다.
	UFUNCTION(Client, Reliable)
	void ClientRPC_RollbackItems(bool bIsPlus, const FDSItemInfo& ItemData, FIntPoint TopLeftIndex);


protected:

	bool TryGetInventoryArrayAndInfo(int32 ItemID, TArray<FDSItemInfo>*& Inventory, FInventoryInfo*& InventoryInfo);
	TMap<EInventoryCategory, TArray<FDSItemInfo>> InventoryArrayMap;

	UPROPERTY(Transient)
	TMap<EInventoryCategory, FInventoryInfo> InventoryInfoMap;

};