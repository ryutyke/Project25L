#pragma once

// Defualt
#include "CoreMinimal.h"

// UE
#include "Animation/DSBaseAnimInstance.h"

// UHT
#include "DSArmedAnimInstance.generated.h"


UCLASS()
class PROJECT25L_API UDSArmedAnimInstance : public UDSBaseAnimInstance
{
	GENERATED_BODY()
	
public:
	UDSArmedAnimInstance();

protected:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
	
	UPROPERTY(Transient, BlueprintReadOnly)
	uint8 bIsEquipped : 1;
};
