#pragma once
// Default
#include "CoreMinimal.h"
// UE
#include "GameFramework/Actor.h"
// UHT
#include "DSSharedInventoryActor.generated.h"

UCLASS()
class PROJECT25L_API ADSSharedInventoryActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADSSharedInventoryActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


private:
	UPROPERTY(EditAnywhere, Category = "DSSettings|Mesh", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> CubeMeshComponent;

	UPROPERTY(EditAnywhere, Category = "DSSettings|Rotate", Meta = (AllowPrivateAccess = "true"))
	float RotateInterval;

	UPROPERTY(EditAnywhere, Category = "DSSettings|Rotate", Meta = (AllowPrivateAccess = "true"))
	float RotateValue;

	FTimerHandle RotateTimerHandle;

	UFUNCTION()
	void RotateCube();
};
