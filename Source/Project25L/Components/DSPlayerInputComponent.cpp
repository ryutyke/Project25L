// Default
#include "Components/DSPlayerInputComponent.h"

// UE
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "InputMappingContext.h"

//Game
#include "Animation/DSBaseAnimInstance.h"
#include "Character/Characters/DSCharacter.h"
#include "Character/Characters/DSCharacter_Girl.h"
#include "Components/DSCharacterMovementComponent.h"
#include "Components/DSFlightComponent.h"
#include "Components/DSInventoryComponent.h"
#include "Components/Skill/DSSkillControlComponent.h"
#include "DSLogChannels.h"
#include "GameData/GameplayTag/DSGameplayTags.h"
#include "HUD/DSHUD.h"
#include "Input/DSInputComponent.h"
#include "Item/DSItemActor.h"
#include "Player/DSPlayerController.h"
#include "System/DSEventSystems.h"
#include "System/DSGameUtils.h"
#include "System/DSUIManagerSubsystem.h"


UDSPlayerInputComponent::UDSPlayerInputComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, CurrentSpeedType(ESpeedType::None)
	, DSInputComponent(nullptr)
	, bIsCrouched(false)
	, PressedStartTime(0.0f)
	, HoldTime(0.3f)
	, bIsInventoryMode(false)
	, MouseSensitivity(1.0f)
{
}

void UDSPlayerInputComponent::SetupInputComponent(UInputComponent* InputComponent)
{
	SetInputMappingContext(EInputMappingContextType::DefaultIMC);

	DSInputComponent = Cast<UDSInputComponent>(InputComponent);

	if (IsValid(DSInputComponent))
	{
		SetCrounchMode(ECrouchMode::ToggleMode);

		// Move
		DSInputComponent->BindSingleActions(InputConfig, FDSTags::GetDSTags().InputTag_Move, ETriggerEvent::Triggered, this, &UDSPlayerInputComponent::Input_Move);
		DSInputComponent->BindSingleActions(InputConfig, FDSTags::GetDSTags().InputTag_Look_Mouse, ETriggerEvent::Triggered, this, &UDSPlayerInputComponent::Input_Look_Mouse);
		DSInputComponent->BindDualActions(InputConfig,	 FDSTags::GetDSTags().InputTag_Jump, this, &UDSPlayerInputComponent::Input_Jump, &UDSPlayerInputComponent::Input_StopJumping);
		DSInputComponent->BindSingleActions(InputConfig, FDSTags::GetDSTags().InputTag_Parkour, ETriggerEvent::Triggered, this, &UDSPlayerInputComponent::Input_Parkour);
		DSInputComponent->BindDualActions(InputConfig,   FDSTags::GetDSTags().InputTag_FastRun, this, &UDSPlayerInputComponent::Input_FastRun, &UDSPlayerInputComponent::Input_StopFastRun);

		// System
		DSInputComponent->BindSingleActions(InputConfig, FDSTags::GetDSTags().InputTag_Pause, ETriggerEvent::Triggered, this, &UDSPlayerInputComponent::Input_Pause);
		DSInputComponent->BindSingleActions(InputConfig, FDSTags::GetDSTags().InputTag_Interaction, ETriggerEvent::Completed, this, &UDSPlayerInputComponent::Input_Interaction);


		// Item
		DSInputComponent->BindSingleActions(InputConfig, FDSTags::GetDSTags().InputTag_Item_QuickSlot1, ETriggerEvent::Triggered, this, &UDSPlayerInputComponent::Input_Item_QuickSlot1);
		DSInputComponent->BindSingleActions(InputConfig, FDSTags::GetDSTags().InputTag_Item_QuickSlot2, ETriggerEvent::Triggered, this, &UDSPlayerInputComponent::Input_Item_QuickSlot2);
		DSInputComponent->BindSingleActions(InputConfig, FDSTags::GetDSTags().InputTag_Item_QuickSlot3, ETriggerEvent::Triggered, this, &UDSPlayerInputComponent::Input_Item_QuickSlot3);
		DSInputComponent->BindSingleActions(InputConfig, FDSTags::GetDSTags().InputTag_Item_Vehicle, ETriggerEvent::Triggered, this, &UDSPlayerInputComponent::Input_Item_Vehicle);

		// Weapon
		DSInputComponent->BindSingleActions(InputConfig, FDSTags::GetDSTags().InputTag_Weapon_Toggle, ETriggerEvent::Triggered, this, &UDSPlayerInputComponent::Input_Weapon_Toggle);

		DSInputComponent->BindSingleActions(InputConfig, FDSTags::GetDSTags().InputTag_Weapon_Equipment_toggle, ETriggerEvent::Started, this, &UDSPlayerInputComponent::Input_Equipment_Toggle);

		// UI
		DSInputComponent->BindSingleActions(InputConfig, FDSTags::GetDSTags().InputTag_UI_Inventory, ETriggerEvent::Started, this, &UDSPlayerInputComponent::Input_UI_Inventory);
		DSInputComponent->BindSingleActions(InputConfig, FDSTags::GetDSTags().InputTag_UI_Status, ETriggerEvent::Triggered, this, &UDSPlayerInputComponent::Input_UI_Status);

		// Skill
		DSInputComponent->BindSkillActions(InputConfig, FDSTags::GetDSTags().InputTag_Skill_PrimarySkill, this,
			&UDSPlayerInputComponent::Input_Weapon_Attack_Started, &UDSPlayerInputComponent::Input_Weapon_Attack_Onging, &UDSPlayerInputComponent::Input_Weapon_Attack_Completed);

		DSInputComponent->BindSkillActions(InputConfig, FDSTags::GetDSTags().InputTag_Skill_Skill1, this,
			&UDSPlayerInputComponent::Input_Weapon_Attack_Started, &UDSPlayerInputComponent::Input_Weapon_Attack_Completed);

		DSInputComponent->BindSkillActions(InputConfig, FDSTags::GetDSTags().InputTag_Skill_Skill2, this,
			&UDSPlayerInputComponent::Input_Weapon_Attack_Started, &UDSPlayerInputComponent::Input_Weapon_Attack_Completed);

		DSInputComponent->BindSkillActions(InputConfig, FDSTags::GetDSTags().InputTag_Skill_UltimateSkill, this,
			&UDSPlayerInputComponent::Input_Weapon_Attack_Started, &UDSPlayerInputComponent::Input_Weapon_Attack_Completed);

		DSInputComponent->BindSkillActions(InputConfig, FDSTags::GetDSTags().InputTag_Skill_FarmingSkill , this,
			&UDSPlayerInputComponent::Input_Weapon_Attack_Started, &UDSPlayerInputComponent::Input_Weapon_Attack_Completed);

		DSInputComponent->BindSkillActions(InputConfig, FDSTags::GetDSTags().InputTag_Skill_DodgeSkill, this,
			&UDSPlayerInputComponent::Input_Weapon_Attack_Started, &UDSPlayerInputComponent::Input_Weapon_Attack_Completed);

		// FlightSkill
		DSInputComponent->BindSingleActions(InputConfig, FDSTags::GetDSTags().InputTag_Skill_Flight_Begin, ETriggerEvent::Started, this, &UDSPlayerInputComponent::Input_Skill_Flight_Begin);
		DSInputComponent->BindSingleActions(InputConfig, FDSTags::GetDSTags().InputTag_Skill_Flight_Up, ETriggerEvent::Triggered, this, &UDSPlayerInputComponent::Input_Skill_Flight_Up);
		DSInputComponent->BindSingleActions(InputConfig, FDSTags::GetDSTags().InputTag_Skill_Flight_Down, ETriggerEvent::Triggered, this, &UDSPlayerInputComponent::Input_Skill_Flight_Down);
	}
}
void UDSPlayerInputComponent::SetInputMappingContext(EInputMappingContextType NewIMCType)
{
	ACharacter* Character = Cast<ACharacter>(GetOwner());

	APlayerController* PlayerController = Character->GetController<APlayerController>();

	if (false == IsValid(PlayerController) || false == IsValid(InputMappingContexts[NewIMCType]))
	{
		return;
	}

	ULocalPlayer * LocalPlayer = PlayerController->GetLocalPlayer();

	if (false == IsValid(LocalPlayer))
	{
		return;
	}

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);
	check(Subsystem);
	
	Subsystem->ClearAllMappings();
	Subsystem->AddMappingContext(InputMappingContexts[NewIMCType], 0);
}


void UDSPlayerInputComponent::OnInputMappingChanged(bool bCanFly, bool bIsCompleted)
{
	if (false == bIsCompleted)
	{
		/* 완료되지 않으면, IMC를 교체하지 않는다.*/
		return;
	}

	const EInputMappingContextType ChangedInputMaapingContextType = EInputMappingContextType::FlightIMC;
	APlayerController* PlayerController = GetController<APlayerController>();

	if (false == IsValid(PlayerController) || false == IsValid(InputMappingContexts[ChangedInputMaapingContextType]))
	{
		return;
	}

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
	check(Subsystem);

	if (bCanFly)
	{
		Subsystem->AddMappingContext(InputMappingContexts[ChangedInputMaapingContextType], 0);
		/*캐릭터 Movement 에게 Movement Mode 변경하도록 전달*/
	}
	else
	{

		Subsystem->RemoveMappingContext(InputMappingContexts[ChangedInputMaapingContextType]);
	}
}

void UDSPlayerInputComponent::InitialCharacterSetting()
{
	bIsCrouched = false;
	bIsInventoryMode = false;
	SetSpeed(ESpeedType::Forward);

}

void UDSPlayerInputComponent::OnUnregister()
{
	Super::OnUnregister();
}

void UDSPlayerInputComponent::Input_Move(const FInputActionValue& InputActionValue)
{
	FVector2D MovementVector = InputActionValue.Get<FVector2D>();

	APlayerController* PlayerController = GetController<APlayerController>();
	
	if (false == IsValid(PlayerController))
	{
		return;
	}
	APawn* Pawn = PlayerController->GetPawn();
	if (false == IsValid(Pawn))
	{
		return;
	}

	const FRotator Rotation = PlayerController->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	ESpeedType NewSpeedType = CalculateSpeed();
	if (CurrentSpeedType != NewSpeedType)
	{
		SetSpeed(NewSpeedType);
		CurrentSpeedType = NewSpeedType;
	}
	
	Pawn = PlayerController->GetPawn();
	Pawn->AddMovementInput(ForwardDirection, MovementVector.Y);
	Pawn->AddMovementInput(RightDirection, MovementVector.X);
}


void UDSPlayerInputComponent::Input_Look_Mouse(const FInputActionValue& InputActionValue)
{
	const FVector2D Value = InputActionValue.Get<FVector2D>();

	APlayerController* PlayerController = GetController<APlayerController>();


	if (false == IsValid(PlayerController))
	{
		return;
	}
	APawn* Pawn = PlayerController->GetPawn();
	if (false == IsValid(Pawn))
	{
		return;
	}

	//Y값 + 20.f ~ -20.f 사이로 제한 (위 아래 -> 빼놓기.)
	
	if (Value.X != 0.0f)
	{
		Pawn->AddControllerYawInput(Value.X * MouseSensitivity);
	}

	if (Value.Y != 0.0f)
	{
		Pawn->AddControllerPitchInput(-Value.Y * MouseSensitivity);
	}
}

void UDSPlayerInputComponent::Input_Jump(const FInputActionValue& InputActionValue)
{
	APlayerController* PlayerController = GetController<APlayerController>();
	if (false == IsValid(PlayerController))
	{
		return;
	}
	APawn* Pawn = PlayerController->GetPawn();
	if (false == IsValid(Pawn))
	{
		return;
	}
	ADSCharacter* Character = Cast<ADSCharacter>(Pawn);
	if (IsValid(Character))
	{
		if (PlayerController->IsInputKeyDown(EKeys::W))
		{
			bIsRun = true;
			Character->SetJumpHeight(bIsRun);
		}
		Character->Jump();
		bIsRun = false;
	}
}

void UDSPlayerInputComponent::Input_StopJumping(const FInputActionValue& InputActionValue)
{
	APlayerController* PlayerController = GetController<APlayerController>();
	if (false == IsValid(PlayerController))
	{
		return;
	}
	APawn* Pawn = PlayerController->GetPawn();
	if (false == IsValid(Pawn))
	{
		return;
	}
	ADSCharacter* Character = Cast<ADSCharacter>(Pawn);
	if (IsValid(Character))
	{
		Character->StopJumping();
		Character->SetJumpHeight(bIsRun);
	}
}

void UDSPlayerInputComponent::Input_Parkour(const FInputActionValue& InputActionValue)
{
}

void UDSPlayerInputComponent::Input_Sit(const FInputActionValue& InputActionValue)
{
	APlayerController* PlayerController = GetController<APlayerController>();
	if (false == IsValid(PlayerController))
	{
		return;
	}
	ADSCharacter* Character = Cast<ADSCharacter>(PlayerController->GetPawn());
	if (IsValid(Character))
	{
		Character->Crouch();
	}
}

void UDSPlayerInputComponent::Input_StopSit(const FInputActionValue& InputActionValue)
{
	APlayerController* PlayerController = GetController<APlayerController>();
	if (false == IsValid(PlayerController))
	{
		return;
	}
	APawn* Pawn = PlayerController->GetPawn();
	if (false == IsValid(Pawn))
	{
		return;
	}
	ADSCharacter* Character = Cast<ADSCharacter>(Pawn);
	if (IsValid(Character))
	{
		Character->UnCrouch();
	}
}

void UDSPlayerInputComponent::Input_ToggleSit(const FInputActionValue& InputActionValue)
{
	APlayerController* PlayerController = GetController<APlayerController>();
	if (false == IsValid(PlayerController))
	{
		return;
	}
	APawn* Pawn = PlayerController->GetPawn();
	if (false == IsValid(Pawn))
	{
		return;
	}
	ADSCharacter* Character = Cast<ADSCharacter>(Pawn);
	if (IsValid(Character))
	{
		if (bIsCrouched)
		{
			Character->UnCrouch(); 
			bIsCrouched = false;
		}
		else
		{
			Character->Crouch();
			bIsCrouched = true;
		}
	}

}

void UDSPlayerInputComponent::Input_FastRun(const FInputActionValue& InputActionValue)
{
	SetSpeed(ESpeedType::Sprint);
	DS_LOG(DSInputLog, Log, TEXT("Sprint !!!\n"));
}

void UDSPlayerInputComponent::Input_StopFastRun(const FInputActionValue& InputActionValue)
{
	SetSpeed(ESpeedType::Forward);
}

void UDSPlayerInputComponent::Input_Pause(const FInputActionValue& InputActionValue)
{
}

void UDSPlayerInputComponent::Input_Interaction(const FInputActionValue& InputActionValue)
{
	//살쒔怨밴묶癒퐣 F袁ⓥ뀮筌
	APlayerController* PlayerController = GetController<APlayerController>();
	if (false == IsValid(PlayerController))
	{
		return;
	}
	
	ADSCharacter* Character = Cast<ADSCharacter>(UDSGameUtils::GetCharacter(PlayerController));
	if (IsValid(Character))
	{
		Character->TryInteraction();
	}
}

void UDSPlayerInputComponent::Input_Item_QuickSlot1(const FInputActionValue& InputActionValue)
{
}

void UDSPlayerInputComponent::Input_Item_QuickSlot2(const FInputActionValue& InputActionValue)
{
}

void UDSPlayerInputComponent::Input_Item_QuickSlot3(const FInputActionValue& InputActionValue)
{
}

void UDSPlayerInputComponent::Input_Item_Vehicle(const FInputActionValue& InputActionValue)
{
}

void UDSPlayerInputComponent::Input_Weapon_Toggle(const FInputActionValue& InputActionValue)
{
}

void UDSPlayerInputComponent::Input_Weapon_Attack_Started(const FInputActionValue& InputActionValue, FGameplayTag InputTag)
{
	ADSCharacter* Character = GetPawn<ADSCharacter>();

	if (IsValid(Character))
	{
		ADSArmedCharacter* ArmedCharacter = Cast<ADSArmedCharacter>(Character);

		if (IsValid(ArmedCharacter))
		{
			bool bIsEquipped = ArmedCharacter->GetIsEquipped();

			// 그 외에는 무기를 꺼낸다.
			if (bIsEquipped == false)
			{
				ArmedCharacter->PlayWeaponActionMontage(EWeaponState::Equipped);
			}
		}

		HoldTime = Character->GetInputThreshold(InputTag);
	}

	UWorld* World = GetWorld();
	check(World);
	PressedStartTime = World->GetTimeSeconds();
}

void UDSPlayerInputComponent::Input_Weapon_Attack_Onging(const FInputActionValue& InputActionValue, FGameplayTag InputTag)
{
	FGameplayTag SkillTag =  ConvertInputTagToSkillTag(InputTag);
	if(SkillTag == FDSTags::GetDSTags().Skill_None)
	{	
		return;
	}
	DefaultAttack(SkillTag);

	ADSCharacter* Character = GetPawn<ADSCharacter>();

	if (IsValid(Character))
	{
		//콤보 실행
		Character->SetNextComboCommand(true);
	}
}

void UDSPlayerInputComponent::Input_Weapon_Attack_Completed(const FInputActionValue& InputActionValue, FGameplayTag InputTag)
{
	FGameplayTag SkillTag =  ConvertInputTagToSkillTag(InputTag);
	if(SkillTag == FDSTags::GetDSTags().Skill_None)
	{	
		return;
	}
	DefaultAttack(SkillTag);
	
	ADSCharacter* Character = GetPawn<ADSCharacter>();

	if (IsValid(Character))
	{
		//콤보 취소
		Character->SetNextComboCommand(false);
	}
}

void UDSPlayerInputComponent::DefaultAttack(FGameplayTag SkillTag)
{
	UWorld* World = GetWorld();
	check(World);
	float PressedTime = World->GetTimeSeconds() - PressedStartTime;

	if (PressedTime >= HoldTime)
	{
		APlayerController* PlayerController = GetController<APlayerController>();

		if (false == IsValid(PlayerController))
		{
			return;
		}
		ADSCharacter* Character = Cast<ADSCharacter>(UDSGameUtils::GetCharacter(PlayerController));
		if (IsValid(Character))
		{
			UDSSkillControlComponent* SkillControlComponent = Character->GetSkillControlComponent();
			if (IsValid(SkillControlComponent))
			{
				if(true == SkillControlComponent->OnSkillPressedEvents.Contains(SkillTag))
				{
					DSEVENT_DELEGATE_INVOKE(SkillControlComponent->OnSkillPressedEvents[SkillTag], SkillTag);
				}
			}
		}
		PressedStartTime = World->GetTimeSeconds();
	}
	
}

void UDSPlayerInputComponent::Input_Equipment_Toggle()
{
	ADSArmedCharacter* Character = GetPawn<ADSArmedCharacter>();
	check(Character);

	bool bIsEquipped = Character->GetIsEquipped();
	if (bIsEquipped)
	{
		//장착을 해제한다.
		Character->PlayWeaponActionMontage(EWeaponState::Unequipped);
	}
	else
	{
		//장착한다.
		Character->PlayWeaponActionMontage(EWeaponState::Equipped);
	}

}

void UDSPlayerInputComponent::Input_UI_Inventory(const FInputActionValue& InputActionValue)
{
	ADSPlayerController* PlayerController = GetController<ADSPlayerController>();
	if (IsValid(PlayerController))
	{
		UDSUIManagerSubsystem* UIManager = UDSUIManagerSubsystem::Get(this);
		check(UIManager);

		if (bIsInventoryMode)
		{
			PlayerController->SetGameFocusMode();
			UIManager->PopContentToLayer(FDSTags::GetDSTags().UI_Layer_GameplayUI_CombatInventoryWidget);
			bIsInventoryMode = false;
		}
		else
		{
			bIsInventoryMode = true;
			UIManager->PushContentToLayer(FDSTags::GetDSTags().UI_Layer_GameplayUI_CombatInventoryWidget);
			PlayerController->SetUIFocusMode();
		}
	}
}

void UDSPlayerInputComponent::Input_UI_Status(const FInputActionValue& InputActionValue)
{
}

void UDSPlayerInputComponent::Input_Skill_Flight_Up(const FInputActionValue& InputActionValue)
{

	UDSFlightComponent* FlightComponent = GetFlightComponent();

	if (false == IsValid(FlightComponent))
	{
		return;
	}

	DSEVENT_DELEGATE_INVOKE(FlightComponent->OnFlightStateChanged, EFlightState::Up);

}

void UDSPlayerInputComponent::Input_Skill_Flight_Down(const FInputActionValue& InputActionValue)
{
	UDSFlightComponent* FlightComponent = GetFlightComponent();

	if (false == IsValid(FlightComponent))
	{
		return;
	}

	DSEVENT_DELEGATE_INVOKE(FlightComponent->OnFlightStateChanged, EFlightState::Down);
}

void UDSPlayerInputComponent::Input_Skill_Flight_Begin(const FInputActionValue& InputActionValue)
{
	UDSFlightComponent* FlightComponent = GetFlightComponent();

	if (false == IsValid(FlightComponent))
	{
		return;
	}

	//Sprint시 회피가 가능하다.
	DSEVENT_DELEGATE_INVOKE(FlightComponent->OnFlightStateChanged, EFlightState::Flighted);
}

void UDSPlayerInputComponent::SetSpeed(ESpeedType TargetWalkSpeed)
{
	APlayerController* PlayerController = GetController<APlayerController>();
	if (false ==  IsValid(PlayerController))
	{
		return;
	}
	APawn* Pawn = PlayerController->GetPawn();
	if (false ==  IsValid(Pawn))
	{
		return;
	}
	ADSCharacter* Character = Cast<ADSCharacter>(Pawn);
	if (IsValid(Character))
	{
		UDSCharacterMovementComponent* DSMovemnt = Cast<UDSCharacterMovementComponent>(Character->GetCharacterMovement());
		if (DSMovemnt)
		{
			DSMovemnt->SetSpeedCommand(TargetWalkSpeed);
		}
	}
}

ESpeedType UDSPlayerInputComponent::CalculateSpeed()
{

	APlayerController* PlayerController = GetController<APlayerController>();
	if (false == IsValid(PlayerController))
	{
		return ESpeedType::None;
	}
	bool bPressedW = PlayerController->IsInputKeyDown(EKeys::W);
	bool bPressedS = PlayerController->IsInputKeyDown(EKeys::S);
	bool bPressedA = PlayerController->IsInputKeyDown(EKeys::A);
	bool bPressedD = PlayerController->IsInputKeyDown(EKeys::D);
	bool bPressedShift = PlayerController->IsInputKeyDown(EKeys::LeftShift);
	
	if ((bPressedW && bPressedS) || (bPressedA && bPressedD))
	{
		return ESpeedType::None;
	}

	if (bPressedShift)
	{
		return ESpeedType::Sprint;
	}

	if (bPressedS)
	{
		return ESpeedType::Backward;
	}
	
	return ESpeedType::Forward;
}


void UDSPlayerInputComponent::SetCrounchMode(ECrouchMode TargetMode)
{
	if (CurrentCrouchMode == TargetMode)
	{
		return;
	}

	const UInputAction* IA = InputConfig->FindNativeInputActionForTag(FDSTags::GetDSTags().InputTag_Sit);

	if (false ==  IsValid(IA))
	{
		return;
	}

	DSInputComponent->RemoveBinds(IA);	

	if (TargetMode == ECrouchMode::HoldMode )
	{
		DSInputComponent->BindDualActions(InputConfig, FDSTags::GetDSTags().InputTag_Sit, this, &UDSPlayerInputComponent::Input_Sit, &UDSPlayerInputComponent::Input_StopSit);
	}
	else if (TargetMode == ECrouchMode::ToggleMode)
	{
		DSInputComponent->BindSingleActions(InputConfig, FDSTags::GetDSTags().InputTag_Sit, ETriggerEvent::Started, this, &UDSPlayerInputComponent::Input_ToggleSit);
	}

	CurrentCrouchMode = TargetMode;
}

UDSFlightComponent* UDSPlayerInputComponent::GetFlightComponent() const
{
	APlayerController* PlayerController = GetController<APlayerController>();
	if (false == IsValid(PlayerController))
	{
		return nullptr;
	}

	ADSCharacter* Character = Cast<ADSCharacter>(UDSGameUtils::GetCharacter(PlayerController));
	if (false == IsValid(Character))
	{
		return nullptr;
	}

	UDSFlightComponent* FlightComponent = Character->GetFlightComponent();

	if (false == IsValid(FlightComponent))
	{
		return nullptr;
	}

	return FlightComponent;
}

FGameplayTag UDSPlayerInputComponent::ConvertInputTagToSkillTag(const FGameplayTag& InputTag)
{
	static const FString InputPrefix = TEXT("InputTag.");

	FString InputTagName = InputTag.ToString();

	if (InputTagName.StartsWith(InputPrefix))
	{
		FString SkillTagName = InputTagName.RightChop(InputPrefix.Len());
		return FGameplayTag::RequestGameplayTag(FName(*SkillTagName));
	}

	return FDSTags::GetDSTags().Skill_None;
}