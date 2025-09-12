#pragma once

// Defualt
#include "CoreMinimal.h"

// UE
#include "GameFramework/HUD.h"
#include "GameplayTagContainer.h"

// Game
#include "GameData/DSEnums.h"

// UHT
#include "DSHUD.generated.h"

class UUserWidget;
class UDSWidgetLayer;
class UDSPrimaryLayout;


UCLASS()
class PROJECT25L_API ADSHUD : public AHUD
{
	GENERATED_BODY()
	
public:

	virtual void BeginPlay() override;
	void InitializeWidgets();
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UDSPrimaryLayout> PrimaryLayoutClass;

};
