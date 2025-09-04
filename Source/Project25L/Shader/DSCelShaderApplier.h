#pragma once

// Default
#include "CoreMinimal.h"

// UE
#include "Materials/MaterialInstanceDynamic.h"
#include "UObject/Object.h"

// UHT
#include "DSCelShaderApplier.generated.h"

/**
 * @brief 월드 및 액터에 카툰 셀 셰이딩 머티리얼을 적용하는 정적 유틸리티 클래스
 */
UCLASS()
class PROJECT25L_API UDSCelShaderApplier final : public UObject
{
	GENERATED_BODY()

public:
	/** 셀 셰이더 초기화 */
	static void Initialize(UObject* WorldContextObject);

	/** 리소스 및 델리게이트 정리 */
	static void Shutdown();

private:
	static void ApplyToWorld(UWorld* World);
	static void ApplyToActor(AActor* TargetActor);
	static UMaterialInstanceDynamic* GetOrCreateInstance(UMaterialInstance* SourceMat);
	static void CopyMaterialParameters(UMaterialInstance* Source, UMaterialInstanceDynamic* Dest);

	static void OnLevelAdded(ULevel* InLevel, UWorld* InWorld);
	static void OnActorSpawned(AActor* SpawnedActor);

private:
	static TSoftObjectPtr<UMaterialInterface> CelShaderAsset;
	static UMaterialInterface* CelShaderMaster;
	static TMap<UTexture*, TWeakObjectPtr<UMaterialInstanceDynamic>> InstanceCache;
	static FDelegateHandle LevelAddHandle;
	static FDelegateHandle ActorSpawnedHandle;
	static FTSTicker::FDelegateHandle TickerHandle;
	static UWorld* CachedWorld;
};