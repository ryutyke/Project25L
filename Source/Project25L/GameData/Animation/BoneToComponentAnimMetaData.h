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
	// 샘플 시간(초)
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<float> Times;

	// 손 본의 컴포넌트 공간 트랜스폼 (Bone -> Component)
	// 회전 정확도를 위해 쿼터니언 유지
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FTransform> BoneToComponent;

	// 간단 보간 헬퍼
	UFUNCTION(BlueprintCallable)
	FTransform GetBoneToComponentTransform(float Time) const;
};
