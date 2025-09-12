// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Default
#include "CoreMinimal.h"

// UE
#include "Blueprint/DragDropOperation.h"

// Game
#include "GameData/Items/DSItemData.h"
#include "GameData/DSEnums.h"

// UHT
#include "DSItemDragDropOperation.generated.h"

UCLASS()
class PROJECT25L_API UDSItemDragDropOperation : public UDragDropOperation
{
	GENERATED_BODY()

public:
    UPROPERTY()
    FDSItemInfo ItemInfo;

    UPROPERTY(Transient, BlueprintReadWrite)
	FIntPoint TopLeftIndex;

    UPROPERTY()
    EInventoryCategory InventoryCategory;

};
