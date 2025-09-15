// Default 
#include "Character/Characters/DSCharacter_Girl.h"

// UE
#include "Camera/CameraComponent.h"
#include "Components/TimelineComponent.h"
#include "EngineUtils.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Net/UnrealNetwork.h"

// Game
#include "Components/DSCameraControlComponent.h"
#include "Components/Input/DSPlayerInputComponent_Girl.h"
#include "Components/Skill/DSSkillControlComponent_Girl.h"
#include "DSLogChannels.h"
#include "Skill/Base/DSTestSkill.h"

#include "System/DSEventSystems.h"

ADSCharacter_Girl::ADSCharacter_Girl(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	SkillControlComponent = CreateDefaultSubobject<UDSSkillControlComponent_Girl>(TEXT("SkillControlComponent"));
	DSPlayerInputComponent = CreateDefaultSubobject<UDSPlayerInputComponent_Girl>(TEXT("DSPlayerInputComponent"));
}

void ADSCharacter_Girl::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// 실제 마우스 스크롤과 관련 => Zoom 이후에 마우스 휠로 땡기거나 밀 수 있다.
	UDSPlayerInputComponent_Girl* GirlPlayerInputComponent = Cast<UDSPlayerInputComponent_Girl>(DSPlayerInputComponent);

	if (IsValid(GirlPlayerInputComponent) && IsValid(CameraControlComponent))
	{
		DSEVENT_DELEGATE_BIND(GirlPlayerInputComponent->OnMouseWheelScrolled, CameraControlComponent, &UDSCameraControlComponent::OnZoomByMouseWheel);
	}

}
