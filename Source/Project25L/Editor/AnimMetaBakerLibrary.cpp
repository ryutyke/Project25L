// Default
#include "AnimMetaBakerLibrary.h"

#if WITH_EDITOR

// UE
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimSingleNodeInstance.h"
#include "Engine/World.h"
#include "ScopedTransaction.h"
#include "Editor.h"
#include "FileHelpers.h"
#include "GameFramework/Character.h"

// Game
#include "GameData/Animation/BoneToComponentAnimMetaData.h"

static UBoneToComponentAnimMetaData* GetOrCreateBoneToComponentMeta(UAnimMontage* InMontage)
{
	if (!InMontage) return nullptr;

	if (UBoneToComponentAnimMetaData* Existing = InMontage->FindMetaDataByClass<UBoneToComponentAnimMetaData>())
	{
		return Existing;
	}

	UBoneToComponentAnimMetaData* NewMeta =	NewObject<UBoneToComponentAnimMetaData>(InMontage, UBoneToComponentAnimMetaData::StaticClass(),	*FString::Printf(TEXT("Meta_BoneToComponent")),	RF_Transactional);

	InMontage->AddMetaData(NewMeta);
	return NewMeta;
}

static UBoneToComponentAnimMetaData* GetOrCreateBoneToComponentMeta(UAnimSequence* InSequence)
{
	if (!InSequence) return nullptr;
	if (UBoneToComponentAnimMetaData* Existing = InSequence->FindMetaDataByClass<UBoneToComponentAnimMetaData>())
		return Existing;

	return NewObject<UBoneToComponentAnimMetaData>(InSequence, UBoneToComponentAnimMetaData::StaticClass(),	*FString::Printf(TEXT("Meta_BoneToComponent")),	RF_Transactional);
}

static UWorld* GetEditorWorld()
{
	if (GEditor)
	{
		return GEditor->GetEditorWorldContext().World();
	}
	return nullptr;
}

#endif // WITH_EDITOR

bool UAnimMetaBakerLibrary::BakeSequenceBoneToComponentMetaAndSave(USkeletalMesh* SkeletalMesh, UAnimSequence* AnimSequence, FName BoneName, float SampleRate)
{
#if WITH_EDITOR
	if (!SkeletalMesh || !AnimSequence || BoneName.IsNone())
	{
		UE_LOG(LogTemp, Warning, TEXT("Bake failed: Missing SkeletalMesh/AnimSequence/BoneName."));
		return false;
	}

	UWorld* World = GetEditorWorld();
	check(IsValid(World));

	USkeletalMeshComponent* SkelComp = NewObject<USkeletalMeshComponent>(World);
	SkelComp->SetSkeletalMesh(SkeletalMesh);
	SkelComp->SetComponentToWorld(FTransform::Identity);
	SkelComp->RegisterComponentWithWorld(World);

	SkelComp->SetAnimationMode(EAnimationMode::AnimationSingleNode);
	SkelComp->SetAnimation(AnimSequence);

	SkelComp->SetUpdateAnimationInEditor(true);
	SkelComp->bEnableUpdateRateOptimizations = false;

	SkelComp->RefreshBoneTransforms();

	UAnimSingleNodeInstance* SingleNode = Cast<UAnimSingleNodeInstance>(SkelComp->GetAnimInstance());
	if (nullptr == SingleNode)
	{
		UE_LOG(LogTemp, Warning, TEXT("Bake Failed: SingleNodeInstance missing"));
		SkelComp->DestroyComponent();
		return false;
	}

	const int32 BoneIndex = SkeletalMesh->GetRefSkeleton().FindBoneIndex(BoneName);
	if (BoneIndex == INDEX_NONE)
	{
		UE_LOG(LogTemp, Warning, TEXT("Bake Failed: bone not found %s"), *BoneName.ToString());
		SkelComp->DestroyComponent();
		return false;
	}

	const FScopedTransaction Tx(NSLOCTEXT("AnimBake", "BakeSequenceBoneToComponent", "Bake BoneToComponent Meta on AnimSequence"));

	AnimSequence->Modify();
	UBoneToComponentAnimMetaData* Meta = GetOrCreateBoneToComponentMeta(AnimSequence);
	if (!Meta)
	{
		UE_LOG(LogTemp, Warning, TEXT("Bake Failed: meta create failed"));
		SkelComp->DestroyComponent();
		return false;
	}
	if (!AnimSequence->GetMetaData().Contains(Meta))
	{
		AnimSequence->AddMetaData(Meta);
	}
	Meta->Modify();
	Meta->Times.Reset();
	Meta->BoneToComponent.Reset();

	Meta->Rate = SampleRate;

	const float Length = AnimSequence->GetPlayLength();
	const float DtSample = (SampleRate > 0.f) ? (1.f / SampleRate) : (1.f / 120.f);

	for (float T = 0.f; T <= Length + KINDA_SMALL_NUMBER; T += DtSample)
	{
		const float ClampedT = FMath::Min(T, Length);

		SingleNode->SetPosition(ClampedT, false);
		SkelComp->RefreshBoneTransforms();

		FTransform BoneToComponentTransfom = FTransform::Identity;
		BoneToComponentTransfom = SkelComp->GetBoneTransform(BoneIndex, FTransform::Identity);

		Meta->Times.Add(ClampedT);
		Meta->BoneToComponent.Add(BoneToComponentTransfom);
	}

	if (UPackage* Pkg = AnimSequence->GetOutermost())
	{
		Pkg->SetDirtyFlag(true);
		TArray<UPackage*> ToSave; ToSave.Add(Pkg);
		UEditorLoadingAndSavingUtils::SavePackages(ToSave, true);
	}

	SkelComp->DestroyComponent();
	return true;
}
#endif // WITH_EDITOR

bool UAnimMetaBakerLibrary::BakeMontageBoneToComponentMetaAndSave(TSubclassOf<ACharacter> OwnerCharacterClass, USkeletalMesh* SkeletalMesh, UAnimMontage* Montage, TSubclassOf<UAnimInstance> AnimBPClass, FName BoneName, float SampleRate /*= 120.f*/)
{
#if WITH_EDITOR
	if (!OwnerCharacterClass || !SkeletalMesh || !Montage || !AnimBPClass || BoneName.IsNone())
	{
		UE_LOG(LogTemp, Warning, TEXT("Bake failed: Missing SkeletalMesh/Montage/AnimBPClass/BoneName."));
		return false;
	}

	UWorld* World = GetEditorWorld();
	if (!World) return false;

	FActorSpawnParameters SpawnParams;
	SpawnParams.ObjectFlags = RF_Transient;
	SpawnParams.bHideFromSceneOutliner = true;
	ACharacter* TempChar = World->SpawnActor<ACharacter>(OwnerCharacterClass, FTransform::Identity, SpawnParams);
	if (!TempChar)
	{
		UE_LOG(LogTemp, Warning, TEXT("Bake failed: Could not spawn owner character."));
		return false;
	}
	TempChar->SetActorHiddenInGame(true);
	TempChar->SetActorEnableCollision(false);

	USkeletalMeshComponent* MeshComp = TempChar->GetMesh();
	if (!MeshComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("Bake failed: Owner character has no Mesh component."));
		TempChar->Destroy();
		return false;
	}

	MeshComp->SetSkeletalMesh(SkeletalMesh);
	MeshComp->SetComponentToWorld(FTransform::Identity);
	MeshComp->RegisterComponentWithWorld(World);
	MeshComp->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	MeshComp->SetAnimInstanceClass(AnimBPClass);
	MeshComp->SetUpdateAnimationInEditor(true);
	MeshComp->bEnableUpdateRateOptimizations = false;
	MeshComp->InitAnim(true);

	UAnimInstance* AnimInst = MeshComp->GetAnimInstance();
	if (!AnimInst)
	{
		UE_LOG(LogTemp, Warning, TEXT("Bake failed: AnimInstance not created from AnimBPClass."));
		TempChar->Destroy();
		return false;
	}

	const FReferenceSkeleton& RefSkel = SkeletalMesh->GetRefSkeleton();
	const int32 BoneIndex = RefSkel.FindBoneIndex(BoneName);
	if (BoneIndex == INDEX_NONE)
	{
		TempChar->Destroy();
		return false;
	}

	const FScopedTransaction Tx(NSLOCTEXT("AnimBake", "BakeMontageBoneToComponent", "Bake BoneToComponent Meta on Montage"));

	Montage->Modify();
	UBoneToComponentAnimMetaData* Meta = GetOrCreateBoneToComponentMeta(Montage);
	if (!Meta)
	{
		Meta = NewObject<UBoneToComponentAnimMetaData>(Montage, UBoneToComponentAnimMetaData::StaticClass(),
			*FString::Printf(TEXT("Meta_BoneToComp_%s"), *Montage->GetName()), RF_Transactional);
		Montage->AddMetaData(Meta);
	}
	Meta->Modify();

	Meta->Times.Reset();
	Meta->BoneToComponent.Reset();

	Meta->Rate = SampleRate;

	const float Length = Montage->GetPlayLength();
	const float Dt = (SampleRate > 0.f) ? (1.f / SampleRate) : (1.f / 60.f);

	AnimInst->Montage_Play(Montage, 0.f);
	
	// float 오차 보정
	for (float T = 0.f; T <= Length + KINDA_SMALL_NUMBER; T += Dt)
	{
		const float ClampedT = FMath::Min(T, Length);
		AnimInst->Montage_SetPosition(Montage, ClampedT);

		MeshComp->TickAnimation(10.f, false);
		MeshComp->RefreshBoneTransforms();
		MeshComp->CompleteParallelAnimationEvaluation(false);

		FTransform BoneToComponentTransform = MeshComp->GetBoneTransform(BoneIndex, FTransform::Identity);

		Meta->Times.Add(ClampedT);
		Meta->BoneToComponent.Add(BoneToComponentTransform);
	}
	AnimInst->Montage_Stop(0.f, Montage);

	UPackage* Pkg = Montage->GetOutermost();
	if (Pkg)
	{
		Pkg->SetDirtyFlag(true);
		TArray<UPackage*> ToSave; ToSave.Add(Pkg);
		UEditorLoadingAndSavingUtils::SavePackages(ToSave, true);
	}

	TempChar->Destroy();
	return true;

#else
	UE_LOG(LogTemp, Warning, TEXT("BakeMontageBoneCSToMetaAndSave is editor-only."));
	return false;

#endif // WITH_EDITOR
}

bool UAnimMetaBakerLibrary::CopyBoneToComponentMetaBetweenMontages(UAnimMontage* SourceMontage, UAnimMontage* TargetMontage)
{
#if WITH_EDITOR
	if (nullptr == SourceMontage || nullptr == TargetMontage)
	{
		UE_LOG(LogTemp, Warning, TEXT("Copy Failed: Missing Montage"));
		return false;
	}

	const UBoneToComponentAnimMetaData* SrcMeta = SourceMontage->FindMetaDataByClass<UBoneToComponentAnimMetaData>();
	if (nullptr == SrcMeta)
	{
		UE_LOG(LogTemp, Warning, TEXT("Copy Failed: Missing Source Meta Data"));
	}

	const FScopedTransaction Tx(NSLOCTEXT("AnimBake", "CopyBoneToComponentMeta", "Copy BoneToComponent Meta Between Montages"));

	TargetMontage->Modify();
	UBoneToComponentAnimMetaData* DstMeta = GetOrCreateBoneToComponentMeta(TargetMontage);
	DstMeta->Modify();

	TArray<float> NewTimes;
	NewTimes.Reserve(SrcMeta->Times.Num());
	TArray<FTransform> NewTransforms;
	NewTransforms.Reserve(SrcMeta->BoneToComponent.Num());

	for (int32 i = 0; i < SrcMeta->Times.Num(); ++i)
	{
		NewTimes.Add(SrcMeta->Times[i]);
		NewTransforms.Add(SrcMeta->BoneToComponent[i]);
	}

	DstMeta->Times = MoveTemp(NewTimes);
	DstMeta->BoneToComponent = MoveTemp(NewTransforms);

	DstMeta->Rate = SrcMeta->Rate;

	auto SavePkg = [](UObject* Obj)
		{
			if (Obj)
			{
				if (UPackage* Pkg = Obj->GetOutermost())
				{
					Pkg->SetDirtyFlag(true);
					TArray<UPackage*> ToSave; ToSave.Add(Pkg);
					UEditorLoadingAndSavingUtils::SavePackages(ToSave, true);
				}
			}
		};

	SavePkg(SourceMontage);
	SavePkg(TargetMontage);

	UE_LOG(LogTemp, Log, TEXT("Copy Bone To Component Meta Data Success"));

	return true;

#else
	UE_LOG(LogTemp, Warning, TEXT("CopyBoneToComponentMetaBetweenMontages is editor-only."));
	return false;
#endif
}
