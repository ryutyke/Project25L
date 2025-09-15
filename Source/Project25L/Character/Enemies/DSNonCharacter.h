#pragma once

// Default
#include "CoreMinimal.h"

// Game
#include "Character/DSCharacterBase.h"

// UHT
#include "DSNonCharacter.generated.h"

class UUserWidget;
class UWidgetComponent;

UCLASS()
class PROJECT25L_API ADSNonCharacter : public ADSCharacterBase
{
	GENERATED_BODY()
	
public:
	ADSNonCharacter(const FObjectInitializer& ObjectInitializer);

	/*Getter/Setter*/
	void InitalizeData(int32 InNonCharacterID);
public:
	//virtual float TakeFinalDamage(float DamageAmount, const FDSDamageEvent& NewDamageEvent, class AController* EventInstigator, AActor* DamageCauser);

protected:
	virtual void Landed(const FHitResult& Hit) override;
	virtual void BeginPlay() override;
protected:
	UPROPERTY(EditAnywhere, Category = "DSSettings | Type")
	int32 NonCharacterID;

	UPROPERTY(EditAnywhere, Category = "DSSettings | Widget")
	TSoftClassPtr<UUserWidget> HPWidgetClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DSSettings | Widget")
	TObjectPtr<UWidgetComponent> HealthComponent;
};
