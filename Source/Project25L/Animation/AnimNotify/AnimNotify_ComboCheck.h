#pragma once

// Default
#include "CoreMinimal.h"

// UE
#include "Animation/AnimNotifies/AnimNotify.h"

// UHT
#include "AnimNotify_ComboCheck.generated.h"


UCLASS()
class PROJECT25L_API UAnimNotify_ComboCheck : public UAnimNotify
{
	GENERATED_BODY()
	
public:
	UAnimNotify_ComboCheck();

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
