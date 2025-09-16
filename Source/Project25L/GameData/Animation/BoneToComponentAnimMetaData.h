#pragma once

// Default
#include "CoreMinimal.h"

// UE
#include "Animation/AnimMetaData.h"

// UHT
#include "BoneToComponentAnimMetaData.generated.h"

UCLASS(BlueprintType)
class PROJECT25L_API UBoneToComponentAnimMetaData : public UAnimMetaData
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Rate;

	// 시간(초)
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<float> Times;

	// 손 본의 컴포넌트 공간 트랜스폼 (Bone -> Component)
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FTransform> BoneToComponent;
};
