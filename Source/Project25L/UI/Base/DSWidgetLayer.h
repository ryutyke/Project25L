#pragma once

// Default
#include "CoreMinimal.h"

// UE
#include "Blueprint/UserWidget.h"
#include "GameplayTagContainer.h"
#include "Interface/UIEventListener.h"

// UHT
#include "DSWidgetLayer.generated.h"

class UBorder;
class UDSUserWidget;

UCLASS()
class PROJECT25L_API UDSWidgetLayer : public UUserWidget, public IUIEventListener
{
	GENERATED_BODY()

public:
	UDSWidgetLayer(const FObjectInitializer& ObjectInitializer);
	virtual void NativeConstruct() override;

	// 위젯을 모두 CreatedWidgetsMap에 저장한다.
	void RegisterWidgets();

	UUserWidget* PushWidget(FGameplayTag WidgetTag);
	void PopWidget();

	void ClearStack();

	void CollapseTopWidget();
	void ShowTopWidget(); 

	UUserWidget* GetTopWidget(); 

	/* Delegate 바인딩하는 함수, PlayerController의 BeginPlay에서 호출된다.*/
	virtual void BindEvents() override ;
	virtual void InitializeWidgets();

protected:

	UPROPERTY(Transient)
	TArray<TObjectPtr<UUserWidget>> Stack; 

	// 생성된 위젯 
	UPROPERTY(Transient)
	TMap<FGameplayTag, TObjectPtr<UDSUserWidget>> CreatedWidgetsMap;

	// 위젯이 각각 가지고 있음 Tag로 찾음
	UPROPERTY(EditAnywhere, Category = "DSSettings | Widget")
	TMap<FGameplayTag, TSoftClassPtr<UUserWidget>> WidgetsMap;
};
