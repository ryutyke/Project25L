#pragma once

// Default
#include "CoreMinimal.h"

// UE
#include "GameFramework/Actor.h"

// UHT
#include "DSGridPlane.generated.h"

UCLASS()
class PROJECT25L_API ADSGridPlane : public AActor
{
	GENERATED_BODY()
	
public:	
	ADSGridPlane();

	FIntPoint GetGridCell(const FVector& TargetLocation);
	FVector GetGridCellCenter(const FIntPoint& Point);
protected:

	UPROPERTY(EditAnywhere, Category = "DSSettings | Grid")
	int32 RowSize;

	UPROPERTY(EditAnywhere, Category = "DSSettings | Grid")
	int32 ColSize;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> Plane;
};
