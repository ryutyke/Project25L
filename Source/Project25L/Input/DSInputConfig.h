#pragma once
//Default
#include "CoreMinimal.h"

//UE
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"

//Game

//UHT
#include "DSInputConfig.generated.h"


/** FDSInputAction
 *
 *	InputAciton과 GameplayTag를 맵핑시켜주기 위해 사용하는 Struct
 */
USTRUCT(BlueprintType)
struct FDSInputAction
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<const UInputAction> InputAction = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (Categories = "InputTag"))
	FGameplayTag InputTag;
};


UCLASS()
class PROJECT25L_API UDSInputConfig : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UDSInputConfig(const FObjectInitializer& ObjectInitializer);

	UFUNCTION()
	const UInputAction* FindNativeInputActionForTag(const FGameplayTag& InputTag) const;
	UFUNCTION()
	const UInputAction* FindDualInputActionsForTag(const FGameplayTag& InputTag) const;
	UFUNCTION()
	const UInputAction* FindTripleInputActionsForTag(const FGameplayTag& InputTag) const;

public:
	UPROPERTY(EditDefaultsOnly, Meta = (TitleProperty = "InputAction"))
	TArray<FDSInputAction> NativeInputActions;
	UPROPERTY(EditDefaultsOnly, Meta = (TitleProperty = "InputAction"))
	TArray<FDSInputAction> DualInputActions;
	UPROPERTY(EditDefaultsOnly, Meta = (TitleProperty = "InputAction"))
	TArray<FDSInputAction> TripleInputActions;

};
