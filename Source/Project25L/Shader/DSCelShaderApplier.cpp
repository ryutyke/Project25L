// Default
#include "DSCelShaderApplier.h"

// UE
#include "Engine/World.h"
#include "Engine/Level.h"
#include "GameFramework/Actor.h"
#include "Components/MeshComponent.h"
#include "Materials/MaterialInstance.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/Engine.h"
#include "EngineUtils.h"
#include "Tickable.h"

// Game
#include "DSLogChannels.h"

namespace {
	static const FName BaseColorParamName(TEXT("BaseColor"));
}

TSoftObjectPtr<UMaterialInterface> UDSCelShaderApplier::CelShaderAsset = TSoftObjectPtr<UMaterialInterface>(FSoftObjectPath(TEXT("MaterialInstanceConstant'/Game/Arts/Shader/CartoonCelShader/Materials/CelShader/MI_CelShader_v2_Opaque_Inst.MI_CelShader_v2_Opaque_Inst'")));
UMaterialInterface* UDSCelShaderApplier::CelShaderMaster = nullptr;
TMap<UTexture*, TWeakObjectPtr<UMaterialInstanceDynamic>> UDSCelShaderApplier::InstanceCache;
FDelegateHandle UDSCelShaderApplier::LevelAddHandle;
FDelegateHandle UDSCelShaderApplier::ActorSpawnedHandle;
FTSTicker::FDelegateHandle UDSCelShaderApplier::TickerHandle;
UWorld* UDSCelShaderApplier::CachedWorld = nullptr;

void UDSCelShaderApplier::Initialize(UObject* WorldContextObject)
{
	if (!IsValid(WorldContextObject)) return;
	CachedWorld = WorldContextObject->GetWorld();
	if (!IsValid(CachedWorld)) return;

	if (!CelShaderMaster)
	{
		if (!CelShaderAsset.IsValid())
		{
			CelShaderAsset.LoadSynchronous();
		}
		CelShaderMaster = CelShaderAsset.Get();
	}

	if (!CelShaderMaster)
	{
		UE_LOG(DSCelShaderLog, Warning, TEXT("[CelShader] Missing CelShaderMaster"));
		return;
	}

	LevelAddHandle = FWorldDelegates::LevelAddedToWorld.AddStatic(&UDSCelShaderApplier::OnLevelAdded);
	ActorSpawnedHandle = CachedWorld->AddOnActorSpawnedHandler(
		FOnActorSpawned::FDelegate::CreateStatic(&UDSCelShaderApplier::OnActorSpawned)
	);

	TickerHandle = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda([](float DeltaTime) -> bool {
		ApplyToWorld(CachedWorld);
		return false;
		}));
}

void UDSCelShaderApplier::Shutdown()
{
	if (LevelAddHandle.IsValid())
	{
		FWorldDelegates::LevelAddedToWorld.Remove(LevelAddHandle);
		LevelAddHandle.Reset();
	}

	if (ActorSpawnedHandle.IsValid() && CachedWorld)
	{
		CachedWorld->RemoveOnActorSpawnedHandler(ActorSpawnedHandle);
		ActorSpawnedHandle.Reset();
	}

	if (TickerHandle.IsValid())
	{
		FTSTicker::GetCoreTicker().RemoveTicker(TickerHandle);
		TickerHandle.Reset();
	}

	InstanceCache.Empty();
	CachedWorld = nullptr;
}

void UDSCelShaderApplier::OnLevelAdded(ULevel* InLevel, UWorld* InWorld)
{
	if (IsValid(InLevel) && IsValid(InWorld))
	{
		ApplyToWorld(InWorld);
	}
}

void UDSCelShaderApplier::OnActorSpawned(AActor* SpawnedActor)
{
	if (IsValid(SpawnedActor) && IsValid(SpawnedActor->GetWorld()))
	{
		ApplyToActor(SpawnedActor);
	}
}

void UDSCelShaderApplier::ApplyToWorld(UWorld* World)
{
	if (!IsValid(World)) return;
	for (TActorIterator<AActor> It(World); It; ++It)
	{
		ApplyToActor(*It);
	}
}

// 쉐이딩 적용할 액터 필터링 해서 적용
void UDSCelShaderApplier::ApplyToActor(AActor* TargetActor)
{
	if (!IsValid(TargetActor)) return;

	if (!TargetActor->ActorHasTag(FName("CelShaderTarget")))
		return;

	TArray<UMeshComponent*> MeshComps;
	TargetActor->GetComponents<UMeshComponent>(MeshComps);

	if (MeshComps.IsEmpty())
	{
		UE_LOG(DSCelShaderLog, Verbose, TEXT("[CelShader] Actor '%s' has NO mesh components"), *TargetActor->GetName());
		return;
	}

	for (UMeshComponent* MeshComp : MeshComps)
	{
		if (!IsValid(MeshComp)) continue;

		const int32 MatCount = MeshComp->GetNumMaterials();
		for (int32 i = 0; i < MatCount; ++i)
		{
			UMaterialInterface* OriginalMat = MeshComp->GetMaterial(i);
			UMaterialInstance* MI = Cast<UMaterialInstance>(OriginalMat);
			if (!IsValid(MI)) continue;

			UTexture* BaseTexture = nullptr;
			MI->GetTextureParameterValue(FMaterialParameterInfo(BaseColorParamName), BaseTexture);
			if (!IsValid(BaseTexture)) continue;

			UMaterialInstanceDynamic* DynInst = GetOrCreateInstance(MI);
			if (IsValid(DynInst))
			{
				MeshComp->SetMaterial(i, DynInst);
				UE_LOG(DSCelShaderLog, Verbose, TEXT("[CelShader] Applied CelShader to '%s' - Mesh: %s"), *TargetActor->GetName(), *MeshComp->GetName());
			}
		}
	}
}

// 머티리얼 인스턴스 생성
UMaterialInstanceDynamic* UDSCelShaderApplier::GetOrCreateInstance(UMaterialInstance* SourceMat)
{
	if (!IsValid(SourceMat)) return nullptr;

	UTexture* BaseTexture = nullptr;
	SourceMat->GetTextureParameterValue(FMaterialParameterInfo(BaseColorParamName), BaseTexture);
	if (!IsValid(BaseTexture)) return nullptr;

	if (TWeakObjectPtr<UMaterialInstanceDynamic>* CachedInst = InstanceCache.Find(BaseTexture))
	{
		if (CachedInst->IsValid())
		{
			return CachedInst->Get();
		}
	}

	UMaterialInstanceDynamic* NewInstance = UMaterialInstanceDynamic::Create(CelShaderMaster, nullptr);
	NewInstance->SetTextureParameterValue(BaseColorParamName, BaseTexture);
	CopyMaterialParameters(SourceMat, NewInstance);

	InstanceCache.Add(BaseTexture, NewInstance);
	return NewInstance;
}

// 머티리얼 파라미터 복사해오기
void UDSCelShaderApplier::CopyMaterialParameters(UMaterialInstance* Source, UMaterialInstanceDynamic* Dest)
{
	if (!IsValid(Source) || !IsValid(Dest)) return;

	TArray<FMaterialParameterInfo> ParamInfos;
	TArray<FGuid> ParamIds;

	// Scalar Params
	Source->GetAllScalarParameterInfo(ParamInfos, ParamIds);
	for (const FMaterialParameterInfo& Info : ParamInfos)
	{
		float Value;
		if (Source->GetScalarParameterValue(Info, Value))
		{
			Dest->SetScalarParameterValue(Info.Name, Value);
		}
	}

	// Vector Params
	Source->GetAllVectorParameterInfo(ParamInfos, ParamIds);
	for (const FMaterialParameterInfo& Info : ParamInfos)
	{
		FLinearColor Color;
		if (Source->GetVectorParameterValue(Info, Color))
		{
			Dest->SetVectorParameterValue(Info.Name, Color);
		}
	}

	// Texture Params
	Source->GetAllTextureParameterInfo(ParamInfos, ParamIds);
	for (const FMaterialParameterInfo& Info : ParamInfos)
	{
		UTexture* Texture;
		if (Source->GetTextureParameterValue(Info, Texture))
		{
			Dest->SetTextureParameterValue(Info.Name, Texture);
		}
	}
}
