// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameData/DSCharacterDataAsset.h"
#include "DSNonCharacterDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT25L_API UDSNonCharacterDataAsset : public UDSActorDataAsset
{
	GENERATED_BODY()

public:
	virtual uint32 GetKey() override;
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DSSettings | Type")
	int32 Type;

};
