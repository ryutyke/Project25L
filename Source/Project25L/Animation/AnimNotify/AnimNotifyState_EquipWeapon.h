#pragma once

// Default
#include "CoreMinimal.h"

// UE
#include "Animation/AnimNotifies/AnimNotifyState.h"

// Game
#include "AnimNotifyState_EquipWeapon.generated.h"


UCLASS()
class PROJECT25L_API UAnimNotifyState_EquipWeapon : public UAnimNotifyState
{
	GENERATED_BODY()
	
public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference);

};
