#pragma once

// Default
#include "CoreMinimal.h"

// UE
#include "GameFramework/Actor.h"

// UHT
#include "DSBead.generated.h"

class UMaterialInstance;

UCLASS()
class PROJECT25L_API ADSBead : public AActor
{
	GENERATED_BODY()
	
public:	
	ADSBead();

	void InitializeMaterial(UMaterialInterface* MaterialInterface);

protected:
	
	UPROPERTY(VisibleAnywhere, Category = "DSSettings | Mesh")
	TObjectPtr<UStaticMeshComponent> Mesh;
};
