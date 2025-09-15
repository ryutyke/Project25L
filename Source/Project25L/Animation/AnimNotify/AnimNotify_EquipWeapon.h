#pragma once

// Default
#include "CoreMinimal.h"

// UE
#include "Animation/AnimNotifies/AnimNotify.h"

// Game
#include "AnimNotify_EquipWeapon.generated.h"


UCLASS()
class PROJECT25L_API UAnimNotify_EquipWeapon : public UAnimNotify
{
	GENERATED_BODY()
	
public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference);

};
