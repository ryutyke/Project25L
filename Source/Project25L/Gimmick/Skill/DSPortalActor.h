#pragma once
// Default
#include "CoreMinimal.h"

// Game
#include "GameFramework/Actor.h"
#include "Skill/Girl/DSSkill2_Girl.h"
#include "Skill/Base/DSSkillActorInfo.h"

// UHT
#include "DSPortalActor.generated.h"

class UNiagaraSystem;
class UMaterialInstance;
class UBoxComponent;

UCLASS()
class PROJECT25L_API ADSPortalActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ADSPortalActor();

	void SetExitLocation(FVector InExitLocation);
	void SetPortalSurfaceType(EDSPortalSurfaceType InPortalSurfaceType);
	void OnPortalSpawned();
	void OnActivatePortal();
	void OnResetPortal();

protected:
	virtual void BeginPlay() override;
	UFUNCTION()
	void OnPortalOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(NetMulticast,Reliable)
	virtual void MulticastRPC_SpawnEffect();

	UFUNCTION(Server, Reliable)
	virtual void ServerRPC_SpawnEffect();

	UFUNCTION(NetMulticast,Reliable)
	virtual void MulticastRPC_DeactivateEffect();

	UFUNCTION(Server, Reliable)
	virtual void ServerRPC_DeactivateEffect();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UBoxComponent> TriggerBox;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<UNiagaraComponent> PortalEffectComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> SphereMeshComponent;

	UPROPERTY(EditAnywhere, Category = "DSSetting|Portal")
	EDSPortalType PortalType;

	UPROPERTY(EditAnywhere, Category = "DSSetting|Portal")
	TObjectPtr<UNiagaraSystem> Effect;

	UPROPERTY(EditAnywhere, Category = "DSSetting|Portal")
	TObjectPtr<UMaterialInstance> MaterialInstance;

	UPROPERTY(EditAnywhere, Category = "DSSetting|DelayTime")
	float TeleportDelayTime;

	UPROPERTY(EditAnywhere, Category = "DSSetting|DelayTime")
	float EndSkillDelayTime;

	FVector ExitLocation;

	uint8 bisPortalActive : 1;

	FTimerHandle TeleportTimerHandle;
	FTimerHandle CancelSkillTimerHandle;

	EDSPortalSurfaceType PortalSurfaceType;
};
