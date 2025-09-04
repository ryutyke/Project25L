#pragma once

// Defualt
#include "CoreMinimal.h"

// UE
#include "Animation/AnimInstance.h"

// UHT
#include "DSGunAnimInstance.generated.h"

class ADSWeapon;
UCLASS()
class PROJECT25L_API UDSGunAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UDSGunAnimInstance();

protected:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:

	UPROPERTY(Transient)
	TObjectPtr<class ADSWeapon> Owner;

	UPROPERTY(Transient)
	uint8 bIsFire : 1;
};
