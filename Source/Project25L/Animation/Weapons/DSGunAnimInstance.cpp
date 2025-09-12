// Default
#include "Animation/Weapons/DSGunAnimInstance.h"

// Game
#include "Weapon/DSWeapon.h"

UDSGunAnimInstance::UDSGunAnimInstance()
	: Super()
	, Owner(nullptr)
	, bIsFire(false)
{
}

void UDSGunAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Owner = Cast<ADSWeapon>(GetOwningActor());
}

void UDSGunAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);


}
