#pragma once

// Default
#include "CoreMinimal.h"

// UE
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameplayTagContainer.h"

// UHT
#include "DSUIManagerSubsystem.generated.h"


class UDSPrimaryLayout;

UCLASS()
class PROJECT25L_API UDSUIManagerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	UDSUIManagerSubsystem();

	static UDSUIManagerSubsystem* Get(UObject* Object);

	UUserWidget* PushContentToLayer(FGameplayTag LayerName);
	void PopContentToLayer(FGameplayTag LayerName);
	void ClearLayer(FGameplayTag LayerName);
	void FocusGame(APlayerController* PlayerController);
	void FocusModal(APlayerController* PlayerController);

	void SetMovementEnable(ACharacter* Character, bool bEnable);
	bool RegisterWidget(UDSPrimaryLayout *Widget);

	UUserWidget* GetTopLayer(FGameplayTag WidgetTag);

	UPROPERTY(Transient)
	TObjectPtr<UDSPrimaryLayout> PrimaryWidget;
};
