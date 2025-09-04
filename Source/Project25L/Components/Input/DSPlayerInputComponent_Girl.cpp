// Default
#include "Components/Input/DSPlayerInputComponent_Girl.h"

// Game
#include "Character/Characters/DSCharacter_Girl.h"
#include "GameData/GameplayTag/DSGameplayTags.h"
#include "Input/DSInputComponent.h"
#include "System/DSEventSystems.h"

#include "DSLogChannels.h"


UDSPlayerInputComponent_Girl::UDSPlayerInputComponent_Girl(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, ActivatedSkill(FGameplayTag::EmptyTag)
{
}

void UDSPlayerInputComponent_Girl::SetupInputComponent(UInputComponent* InputComponent)
{
	Super::SetupInputComponent(InputComponent);
	DSInputComponent = Cast<UDSInputComponent>(InputComponent);

	if (IsValid(DSInputComponent))
	{
		//캐릭터에서 ZoomIn/ZoomOut 관리, 스킬에서는 강력한 총알 발사
		//Started => 줌 인
		//Completed => 줌 아웃 (3초 지났으면 강력한 총알 발사)
		DSInputComponent->BindSkillActions(InputConfig, FDSTags::GetDSTags().InputTag_Skill_SecondarySkill, this,
			&UDSPlayerInputComponent_Girl::Input_Weapon_Attack_Started_Child, &UDSPlayerInputComponent_Girl::Input_Weapon_Attack_Completed_Child);

		DSInputComponent->BindSingleActions(InputConfig, FDSTags::GetDSTags().InputTag_Zoom, ETriggerEvent::Triggered, this, &UDSPlayerInputComponent_Girl::Input_Zoom);
	}
}

void UDSPlayerInputComponent_Girl::Input_Weapon_Attack_Started(const FInputActionValue& InputActionValue, FGameplayTag InputTag)
{
	//InputTag 우선순위를 정한다. ESecondary - EPrimarySkill / ERSkill - EPrimarySkill 같이 사용.
	if (ActivatedSkill != FGameplayTag::EmptyTag)
	{
		//우측키 또는 R키와 같이 사용 중이다.
		InputTag = ActivatedSkill; //교체한다.
	}

	Super::Input_Weapon_Attack_Started(InputActionValue, InputTag);
}

void UDSPlayerInputComponent_Girl::Input_Weapon_Attack_Onging(const FInputActionValue& InputActionValue, FGameplayTag InputTag)
{
	//InputTag 우선순위를 정한다. ESecondary - EPrimarySkill / ERSkill - EPrimarySkill 같이 사용.
	//ERSkill의 경우 총 쏘는 InputThreshold값만 조절하면되기 때문에, R키 사용에 대한 체크를 하지않는다.
	if (ActivatedSkill != FGameplayTag::EmptyTag)
	{
		//우측키 또는 R키와 같이 사용 중이다.
		InputTag = ActivatedSkill; //교체한다.
	}

	Super::Input_Weapon_Attack_Onging(InputActionValue, InputTag);
}

void UDSPlayerInputComponent_Girl::Input_Weapon_Attack_Completed(const FInputActionValue& InputActionValue, FGameplayTag InputTag)
{
	//InputTag 우선순위를 정한다. ESecondary - EPrimarySkill / ERSkill - EPrimarySkill 같이 사용.
	if (ActivatedSkill != FGameplayTag::EmptyTag)
	{
		//우측키 또는 R키와 같이 사용 중이다.
		InputTag = ActivatedSkill; //교체한다.
	}

	Super::Input_Weapon_Attack_Completed(InputActionValue, InputTag);
}

void UDSPlayerInputComponent_Girl::Input_Weapon_Attack_Started_Child(const FInputActionValue& InputActionValue, FGameplayTag InputTag)
{
	//줌을 땡긴다.
	ADSCharacter_Girl* Character = Cast<ADSCharacter_Girl>(GetOwner());

	if (IsValid(Character))
	{
		DSEVENT_DELEGATE_INVOKE(Character->OnZoomInStarted);
	}

	ActivatedSkill = InputTag;
	Super::Input_Weapon_Attack_Started(InputActionValue, InputTag);
}

void UDSPlayerInputComponent_Girl::Input_Weapon_Attack_Completed_Child(const FInputActionValue& InputActionValue, FGameplayTag InputTag)
{
	//줌을 푼다.
	ADSCharacter_Girl* Character = Cast<ADSCharacter_Girl>(GetOwner());

	if (IsValid(Character))
	{
		DSEVENT_DELEGATE_INVOKE(Character->OnZoomOutStarted);
	}

	ActivatedSkill = FGameplayTag::EmptyTag;
}

void UDSPlayerInputComponent_Girl::Input_Zoom(const FInputActionValue& InputActionValue)
{
	const FVector2D Value = InputActionValue.Get<FVector2D>();

	//OnMouseWheelScrolled Invoke
	DSEVENT_DELEGATE_INVOKE(OnMouseWheelScrolled, -Value.X);
}
