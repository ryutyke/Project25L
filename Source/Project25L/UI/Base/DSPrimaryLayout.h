#pragma once

// Default
#include "CoreMinimal.h"

// UE
#include "Blueprint/UserWidget.h"
#include "GameplayTagContainer.h"


// UHT
#include "DSPrimaryLayout.generated.h"

class UDSWidgetLayer;

UCLASS()
class PROJECT25L_API UDSPrimaryLayout : public UUserWidget
{
	GENERATED_BODY()

public:
	UDSPrimaryLayout(const FObjectInitializer& ObjectInitializer);
public:
	virtual void NativeConstruct() override;
	
	UFUNCTION(BlueprintCallable, Category = "DSSettings | Widgets")
	UUserWidget* PushContentToLayer(FGameplayTag WidgetTag);

	UFUNCTION(BlueprintCallable, Category = "DSSettings | Widgets")
	void PopContentfromLayer(FGameplayTag WidgetTag);

	UFUNCTION(BlueprintCallable, Category = "DSSettings | Widgets")
	void ClearLayer(FGameplayTag WidgetTag);

	UFUNCTION(BlueprintCallable, Category = "DSSettings | Widgets")
	UDSWidgetLayer* FindLayerByTag(FGameplayTag WidgetTag);

	void RegisterLayers();
	void BindEvents();

private:

	UPROPERTY(Transient)
	TMap<FGameplayTag, TObjectPtr<UDSWidgetLayer>> LayersMap;

	UPROPERTY(EditAnywhere, meta = (BindWidgetOptions))
	TObjectPtr<UDSWidgetLayer> PersistentLayer;

	// 인벤토리 및 퀘스트 화면, 게임 플레이 메뉴
	UPROPERTY(EditAnywhere, meta = (BindWidgetOptional))
	TObjectPtr<UDSWidgetLayer> GameplayUILayer;
	
	// 팝업 및 대화상자
	UPROPERTY(EditAnywhere, meta = (BindWidgetOptional))
	TObjectPtr<UDSWidgetLayer> ModalLayer;

};
