#pragma once

// Default
#include "CoreMinimal.h"

// UE
#include "Kismet/BlueprintFunctionLibrary.h"

// UHT
#include "AnimMetaBakerLibrary.generated.h"

class UAnimMontage;
class UAnimSequence;
class USkeletalMesh;
class UBoneToComponentAnimMetaData;
class UWorld;

UCLASS()
class PROJECT25L_API UAnimMetaBakerLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, CallInEditor, Category="Anim|Baker", meta=(DevelopmentOnly))
	static bool BakeSequenceBoneToComponentMetaAndSave(USkeletalMesh* SkeletalMesh, UAnimSequence* AnimSequence, FName BoneName, float SampleRate);

	UFUNCTION(BlueprintCallable, CallInEditor, Category="Anim|Baker", meta=(DevelopmentOnly))
	static bool BakeMontageBoneToComponentMetaAndSave(TSubclassOf<ACharacter> OwnerCharacterClass, USkeletalMesh* SkeletalMesh, UAnimMontage* Montage, TSubclassOf<UAnimInstance> AnimBPClass, FName BoneName, float SampleRate = 120.f);

	UFUNCTION(BlueprintCallable, CallInEditor, Category="Anim|Baker", meta=(DevelopmentOnly))
	static bool CopyBoneToComponentMetaBetweenMontages(UAnimMontage* SourceMontage,UAnimMontage* TargetMontage);

};