#pragma once

// Default
#include "CoreMinimal.h"

// UE
#include "Animation/AnimNotifies/AnimNotifyState.h"

// UHT
#include "AnimNotifyState_AttackHitCheck.generated.h"

enum class ESkillType : uint8;

UCLASS()
class PROJECT25L_API UAnimNotifyState_AttackHitCheck : public UAnimNotifyState
{
	GENERATED_BODY()
	
public:
	UAnimNotifyState_AttackHitCheck();

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

protected:

	UPROPERTY(Transient)
	uint8 bHitCheckOnTick : 1;

	UPROPERTY(Transient)
	int32 MaxSequenceCount;
	
	/*Hit Checking 용 Socket*/
	UPROPERTY(EditAnywhere, Category = "DSSettings | AnimationSequence")
	FName HitSocket;

	UPROPERTY(Transient)
	ESkillType SkillType;
};
