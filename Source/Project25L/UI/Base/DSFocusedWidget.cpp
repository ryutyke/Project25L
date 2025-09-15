// Default
#include "UI/Base/DSFocusedWidget.h"

// UE
#include "Components/Image.h"

// Game
#include "System/DSGameDataSubsystem.h"
UDSFocusedWidget::UDSFocusedWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, FocusedIcon(nullptr)
	, DefaultIcon(nullptr)
{
}

void UDSFocusedWidget::NativeConstruct()
{
	Super::NativeConstruct();

	FocusedIcon = LoadTexture(FocusedTexture);
	DefaultIcon = LoadTexture(DefaultTexture);
}

UTexture2D* UDSFocusedWidget::LoadTexture(TSoftObjectPtr<UTexture2D>& Data)
{
	UDSGameDataSubsystem::StreamableManager.LoadSynchronous(Data);

	return Data.Get();
}

void UDSFocusedWidget::SetFocus(UImage* Image, bool bIsFocus)
{

	if (false == IsValid(Image) || false == IsValid(FocusedIcon) || false == IsValid(DefaultIcon))
	{
		return;
	}

	if (bIsFocus)
	{
		Image->SetBrushFromTexture(FocusedIcon);
	}
	else
	{
		Image->SetBrushFromTexture(DefaultIcon);
	}
}
