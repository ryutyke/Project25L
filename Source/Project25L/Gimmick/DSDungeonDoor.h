
#pragma once

// Default
#include "CoreMinimal.h"

// UE
#include "GameFramework/Actor.h"

// Game
#include "GameData/DSEnums.h"

// UHT
#include "DSDungeonDoor.generated.h"

class ADSDungeonMat;
class UBoxComponent;
class UStaticMeshComponent;
class UMaterialInterface;
class UDSDoorData;
class AChaosCacheManager;

/*
* 입장 시 DungeonDoor - DungeonMat 같이 사용
* 퇴장 시 DungeonDoor만 사용
* HUB에서 이미 다녀온 던전은 Chaos Destuction을 사용해서 부서져 
* 있고, 문 Mesh의 중간에 Height/Width 2분의 1 위치에 구슬 Mesh를 띄운다. (그 전까진 숨기고 있음)
*/
UCLASS()
class PROJECT25L_API ADSDungeonDoor : public AActor
{
	GENERATED_BODY()
	
public:	
	ADSDungeonDoor();

	void UnlockDoor(EDungeonType CurrentType);
	void TryDestroyDoor(EDungeonType CurrentType);

protected:

	void InitializeStage();
	
	UFUNCTION()
	void DestroyDoor();
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:

	UPROPERTY(Transient)
	TObjectPtr<ADSDungeonMat> MatVolume;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DSSettings | Lock")
	uint8 bIsUnlocked : 1;

	UPROPERTY(EditAnywhere, Category = "DSSettings | GameData")
	TObjectPtr<UDSDoorData> DungeonDoorData;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DSSettings | Mesh")
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DSSettings | Mesh")
	TObjectPtr<UStaticMeshComponent> Border;

	UPROPERTY(VisibleAnywhere, Category = "DSSettings | Collision")
	TObjectPtr<UBoxComponent> Trigger;
};
