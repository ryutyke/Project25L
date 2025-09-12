#pragma once

// Default
#include "CoreMinimal.h"

// UE
#include "GameplayTagContainer.h"

// Game
#include "Item/DSItemActor.h"
#include "GameData/Items/DSItemData.h"

// UHT
#include "DSGiftBox.generated.h"

class UUserWidget;
class UDSItemList;

UCLASS()
class PROJECT25L_API ADSGiftBox : public ADSItemActor
{
	GENERATED_BODY()
	
public:
	ADSGiftBox();

	// 어떤 아이템이 있는지 띄우기 위한 용도
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnUpdateItemWidget, TArray<FDSItemInfo>&);
	FOnUpdateItemWidget OnUpdateItemWidget;

	DECLARE_MULTICAST_DELEGATE_OneParam(FOnRemoveItemWidget, int32 IndexToRemove);
	FOnRemoveItemWidget OnRemoveItemWidget;

	void RemoveItemData(int32 IndexToRemove);

	FGameplayTag GetWidgetTage() { return ListWidgetTag; }

	void RemoveItemUI(int32 IndexToRemove);

	void PushItemListWidget(ADSCharacter* Character);

protected:
	
	UFUNCTION()
	void OnComponentBeginOverlap_Child(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnComponentEndOverlap_Child(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

protected:

	UPROPERTY(Transient)
	TObjectPtr<UDSItemList> ItemListWidget;

	UPROPERTY(EditAnywhere, Category = "DSSettings | GameTag")
	FGameplayTag  ListWidgetTag;


};
