// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameData/DSPrimaryDataAsset.h"
#include "DSComboActionData.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT25L_API UDSComboActionData : public UDSPrimaryDataAsset
{
	GENERATED_BODY()
	public:
	UDSComboActionData() { }

	UPROPERTY(EditAnywhere, Category = Montage)
	TSoftObjectPtr<UAnimMontage> PunchMontage;

	UPROPERTY(EditAnywhere, Category = ComboData)
	FString MontageSectionNamePrefix;

	UPROPERTY(EditAnywhere, Category = ComboData)
	uint8 MaxComboCount;

	UPROPERTY(EditAnywhere, Category = ComboData)
	float FrameRate;

	UPROPERTY(EditAnywhere, Category = ComboData)
	TArray<float> EffectiveFrameCount;
};
