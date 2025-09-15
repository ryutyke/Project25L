// Default
#include "UI/Base/DSWidgetLayer.h"

// UE
#include "Components/Border.h"

// Game
#include "DSLogChannels.h"
#include "System/DSEventSystems.h"
#include "DSUserWidget.h"

UDSWidgetLayer::UDSWidgetLayer(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, Stack()
	, CreatedWidgetsMap()
{
}

void UDSWidgetLayer::NativeConstruct()
{
	Super::NativeConstruct();

	RegisterWidgets();

	DSEVENT_DELEGATE_BIND(GameEvent.OnCharacterSpawned, this, &UDSWidgetLayer::InitializeWidgets);
}

void UDSWidgetLayer::RegisterWidgets()
{
	for (const auto& Pair : WidgetsMap)
	{
		TSoftClassPtr<UUserWidget> WidgetClass = Pair.Value;

		UClass* LoadedClass = WidgetClass.Get();
		if (false == IsValid(LoadedClass))
		{
			LoadedClass = WidgetClass.LoadSynchronous();
		}

		UDSUserWidget* NewWidget = CreateWidget<UDSUserWidget>(GetOwningPlayer(), LoadedClass);
		ensure(NewWidget != nullptr);
		if (false == IsValid(NewWidget))
		{
			return;
		}
		NewWidget->AddToViewport();
		NewWidget->SetVisibility(ESlateVisibility::Collapsed);
		CreatedWidgetsMap.Add(Pair.Key, NewWidget);
	}
}



UUserWidget* UDSWidgetLayer::PushWidget(FGameplayTag WidgetTag)
{
	
	UUserWidget* NewWidget = nullptr;

	if (CreatedWidgetsMap.Contains(WidgetTag))
	{
		NewWidget = CreatedWidgetsMap[WidgetTag];
	}

	CollapseTopWidget();
	if (IsValid(NewWidget))
	{
		NewWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}

	Stack.Add(NewWidget);
	
	ShowTopWidget();
	return NewWidget;

}



void UDSWidgetLayer::PopWidget()
{

	if (Stack.Num() == 0)
	{	
		return;
	}

	UUserWidget * TopWidget = GetTopWidget();

	if (IsValid(TopWidget))
	{
		TopWidget->SetVisibility(ESlateVisibility::Collapsed);
		if (Stack.Num() > 0)
		{
			Stack.RemoveAt(Stack.Num() - 1);
		}
	}
	UUserWidget* NewTopWidget = GetTopWidget();
	if (IsValid(NewTopWidget))
	{
		NewTopWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
}

void UDSWidgetLayer::ClearStack()
{
	for (UUserWidget* Widget : Stack)
	{
		if (IsValid(Widget))
		{
			Widget->RemoveFromParent();
		}
	}

	Stack.Empty();
}

void UDSWidgetLayer::CollapseTopWidget()
{
	if (Stack.Num() > 0)
	{
		UUserWidget* TopWidget = Stack.Last();
		if (IsValid(TopWidget))
		{
			TopWidget->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void UDSWidgetLayer::ShowTopWidget()
{
	if (Stack.Num() > 0)
	{
		UUserWidget* TopWidget = Stack.Last();  
		if (IsValid(TopWidget))
		{
			TopWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
	}
}

UUserWidget* UDSWidgetLayer::GetTopWidget()
{
	if (Stack.Num() == 0)
	{
		return nullptr;
	}
	return Stack.Last();
}

void UDSWidgetLayer::BindEvents()
{
	for(const auto& Widget : CreatedWidgetsMap)
	{
		if(IsValid(Widget.Value))
		{
			Widget.Value->BindEvents();
		}
	}
}

void UDSWidgetLayer::InitializeWidgets()
{
	for(const auto& Widget : CreatedWidgetsMap)
	{
		if(IsValid(Widget.Value))
		{
			Widget.Value->InitializeWidget();
		}
	}
}

