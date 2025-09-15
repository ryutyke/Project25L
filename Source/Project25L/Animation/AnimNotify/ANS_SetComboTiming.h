#pragma once

// Default
#include "CoreMinimal.h"

// UE
#include "Animation/AnimNotifies/AnimNotifyState.h"

// UHT
#include "ANS_SetComboTiming.generated.h"


UCLASS()
class PROJECT25L_API UANS_SetComboTiming : public UAnimNotifyState
{
	GENERATED_BODY()
	
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
