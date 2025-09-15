// Default
#include "Item/DSGiftBox.h"

// UE
#include "Components/SphereComponent.h"

// Game
#include "Character/Characters/DSCharacter.h"
#include "System/DSEventSystems.h"
#include "System/DSGameUtils.h"
#include "system/DSUIManagerSubsystem.h"
#include "UI/Base/DSWidgetLayer.h"
#include "UI/Item/DSItemList.h"

#include "DSLogChannels.h"

ADSGiftBox::ADSGiftBox()
	: Super()
	, ItemListWidget(nullptr)
{
	Trigger->OnComponentBeginOverlap.AddDynamic(this, &ADSGiftBox::OnComponentBeginOverlap_Child);
	Trigger->OnComponentEndOverlap.AddDynamic(this, &ADSGiftBox::OnComponentEndOverlap_Child);
}

void ADSGiftBox::RemoveItemData(int32 IndexToRemove)
{
	if (StoredItems.IsValidIndex(IndexToRemove))
	{
		StoredItems.RemoveAt(IndexToRemove); //제거한다.
		if (StoredItems.Num() <= 0)
		{
			SetLifeSpan(0.5f);
		}
	}
}

void ADSGiftBox::RemoveItemUI(int32 IndexToRemove)
{
	if (StoredItems.IsValidIndex(IndexToRemove))
	{
		if (IsValid(OverlappedCharacter))
		{
			// if IsLocallyControlled
			//클라이언트 -> UI를 반영
			bool bRes = UDSGameUtils::IsLocallyExecutable(OverlappedCharacter);

			if (bRes)
			{
				//그리고 UI를 업데이트 할 수 있도록 델리게이트나 Widget 업데이트 요청을 한다 => 각 로컬에서만 동작하도록 한다.
				DSEVENT_DELEGATE_INVOKE(OnRemoveItemWidget, IndexToRemove);
			}
		}
	}
}

void ADSGiftBox::PushItemListWidget(ADSCharacter* Character)
{
	if (false == IsValid(Character))
	{
		return ;
	}

	if (Character->IsLocallyControlled())
	{
		UDSUIManagerSubsystem* UIManager = UDSUIManagerSubsystem::Get(this);
		check(UIManager);

		ItemListWidget = Cast<UDSItemList>(UIManager->PushContentToLayer(ListWidgetTag));
	}

	bool bRes = UDSGameUtils::IsLocallyExecutable(Character);

	if (bRes)
	{
		DSEVENT_DELEGATE_BIND(OnUpdateItemWidget, ItemListWidget, &UDSItemList::AddItems);
		DSEVENT_DELEGATE_BIND(OnRemoveItemWidget, ItemListWidget, &UDSItemList::RemoveItem);
	}
}

void ADSGiftBox::OnComponentBeginOverlap_Child(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// ADSCharacter* Character = Cast<ADSCharacter>(OtherActor);
	// if (false == IsValid(Character))
	// {
	// 	return;
	// }
	// 
	// if (Character->IsLocallyControlled())
	// {
	// 	UDSUIManagerSubsystem* UIManager = UDSUIManagerSubsystem::Get(this);
	// 	check(UIManager);
	// 
	// 	ItemListWidget = Cast<UDSItemList>(UIManager->PushContentToLayer(ListWidgetTag));
	// }
	// 
	// bool bRes = UDSGameUtils::IsLocallyExecutable(Character);
	// 
	// if (bRes)
	// {
	// 	DSEVENT_DELEGATE_BIND(OnUpdateItemWidget, ItemListWidget, &UDSItemList::AddItems);
	// 	DSEVENT_DELEGATE_BIND(OnRemoveItemWidget, ItemListWidget, &UDSItemList::RemoveItem);
	// }
}

void ADSGiftBox::OnComponentEndOverlap_Child(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

	ADSCharacter* Character = Cast<ADSCharacter>(OtherActor);
	if (false == IsValid(Character))
	{
		return;
	}

	if (Character->IsLocallyControlled())
	{
		//UI 띄우는 작업이 사라짐
		UDSUIManagerSubsystem* UIManager = UDSUIManagerSubsystem::Get(this);

		check(UIManager);

		UIManager->PopContentToLayer(ListWidgetTag);

		APlayerController* PlayerController = Character->GetController<APlayerController>();

		if (IsValid(PlayerController))
		{
			UIManager->FocusGame(PlayerController);
		}
	}


	DSEVENT_DELEGATE_REMOVE(OnUpdateItemWidget, ItemListWidget);
	DSEVENT_DELEGATE_REMOVE(OnRemoveItemWidget, ItemListWidget);
	
	ItemListWidget = nullptr;

}

