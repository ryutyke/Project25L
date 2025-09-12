// Default
#include "UI/Base/DSPrimaryLayout.h"

// UE
#include "GameFramework/PlayerController.h"


// Game
#include "System/DSUIManagerSubsystem.h"
#include "DSLogChannels.h"
#include "UI/Base/DSWidgetLayer.h"
#include "GameData/GameplayTag/DSGameplayTags.h"
#include "Character/Characters/DSCharacter.h"


UDSPrimaryLayout::UDSPrimaryLayout(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer)
	, LayersMap()
{

}

void UDSPrimaryLayout::NativeConstruct()
{
    Super::NativeConstruct();

	RegisterLayers();
}

UDSWidgetLayer* UDSPrimaryLayout::FindLayerByTag(FGameplayTag WidgetTag)
{
	for (const auto& Pair : LayersMap)
	{
		if (WidgetTag.MatchesTag(Pair.Key)) 
		{
			return Pair.Value;
		}
	}
	return nullptr;
}

UUserWidget* UDSPrimaryLayout::PushContentToLayer(FGameplayTag WidgetTag)
{

	UDSWidgetLayer* FindLayer = FindLayerByTag(WidgetTag);

	if (IsValid(FindLayer))
	{
		return FindLayer->PushWidget(WidgetTag);
	}

	return nullptr;
}

void UDSPrimaryLayout::PopContentfromLayer(FGameplayTag WidgetTag)
{
	UDSWidgetLayer* FindLayer = FindLayerByTag(WidgetTag);

	if (IsValid(FindLayer))
	{
		FindLayer->PopWidget();
	}
}



void UDSPrimaryLayout::ClearLayer(FGameplayTag WidgetTag)
{
	UDSWidgetLayer* FindLayer = FindLayerByTag(WidgetTag);
	if (IsValid(FindLayer))
	{
		FindLayer->ClearStack();
	}
}

void UDSPrimaryLayout::BindEvents()
{
	ADSCharacter* Pawn = Cast<ADSCharacter>(GetOwningPlayerPawn());
	APlayerController* PC = GetOwningPlayer();
	check(PC);

	if (false == IsValid(Pawn))
	{
		FTimerHandle BindEventTimerHandle;
		PC->GetWorld()->GetTimerManager().SetTimer(BindEventTimerHandle, this, &UDSPrimaryLayout::BindEvents, 0.5f, false);
		return;
	}

	for(const auto& Layer:LayersMap)
	{
		Layer.Value->BindEvents();
	}
}

void UDSPrimaryLayout::RegisterLayers()
{
	LayersMap.Empty();

	if (IsValid(GameplayUILayer))
	{
		LayersMap.Add(FDSTags::GetDSTags().UI_Layer_GameplayUI, GameplayUILayer);
	}
	if (IsValid(ModalLayer))
	{
		LayersMap.Add(FDSTags::GetDSTags().UI_Layer_Modal, ModalLayer);
	}
	if (IsValid(PersistentLayer))
	{
		LayersMap.Add(FDSTags::GetDSTags().UI_Layer_Persistent, PersistentLayer);
	}

	if (IsValid(PersistentLayer))
	{
		PersistentLayer->PushWidget(FDSTags::GetDSTags().UI_Layer_Persistent_Main);
	}
}

