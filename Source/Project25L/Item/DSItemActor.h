#pragma once
// Default
#include "CoreMinimal.h"

// UE
#include "GameFramework/Actor.h"

// Game
#include "GameData/DSEnums.h"

// UHT
#include "DSItemActor.generated.h"


class ADSCharacter;
class USphereComponent;
class UStaticMeshComponent;
class UMaterialInterface;
class UWidgetComponent;

struct FDSItemInfo;

UCLASS()
class PROJECT25L_API ADSItemActor : public AActor
{
	GENERATED_BODY()
public:

	ADSItemActor();

	int32 GetID() { return ItemID; }

	int32 GetID() const { return ItemID; }
	const TArray<FDSItemInfo>& GetItemData() const { return StoredItems; }
	EInteractType GetInteractType() { return InteractType; }

public:
	void InitializeItemData(TMap<int32, int32> ItemData);

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:                  

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

protected:
	void InitLocation();

	UFUNCTION()
	void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

protected:
	//위젯 컴포넌트를 만든다.

	UPROPERTY(Transient)
	bool bIsOverlapped;

	UPROPERTY(Transient)
	TObjectPtr<ADSCharacter> OverlappedCharacter;

	//ItemID가 보물 상자일 경우에, StoredItems에 보관한다.
	UPROPERTY(Replicated, Transient)
	TArray<FDSItemInfo> StoredItems;

protected:
	UPROPERTY(EditAnywhere, Category = "DSSettings | Item")
	EInteractType InteractType;

	UPROPERTY(EditAnywhere, Category = "DSSettings | Item")
	int32 ItemID;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DSSettings | Outline")
	TObjectPtr<UMaterialInterface> OutlineMaterial;

	UPROPERTY(VisibleAnywhere, Category = "DSSettings | Mesh")
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "DSSettings | Collision")
	TObjectPtr<USphereComponent> Trigger;


};
