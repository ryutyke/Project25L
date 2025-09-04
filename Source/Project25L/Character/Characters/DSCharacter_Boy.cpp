// Default
#include "Character/Characters/DSCharacter_Boy.h"

// Game
#include "Components/Skill/DSSkillControlComponent_Boy.h"
#include "Components/Input/DSPlayerInputComponent_Boy.h"

#include "DSLogChannels.h"

ADSCharacter_Boy::ADSCharacter_Boy(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SkillControlComponent = CreateDefaultSubobject<UDSSkillControlComponent_Boy>(TEXT("SkillControlComponent"));
	DSPlayerInputComponent = CreateDefaultSubobject<UDSPlayerInputComponent_Boy>(TEXT("DSPlayerInputComponent"));

}

void ADSCharacter_Boy::BeginPrimarySkill()
{
	if (false == bIsEquipped)
	{
		return;
	}
	
	UAnimInstance* AnimInstance = GetAnimInstance();
	if (IsValid(AnimInstance))
	{
		if (AnimInstance->IsAnyMontagePlaying())
		{
			return;
		}
	}
	InitializeComboCount();

	PlayWeaponActionMontage(EWeaponState::PrimaryAttack);
}

void ADSCharacter_Boy::JumpToNextPrimaryAttackCombo(int32 MaxSequenceCount)
{
	if (false == WeaponMontages.Contains(EWeaponState::PrimaryAttack))
	{
		return;
	}

	UAnimMontage* ComboMontage = WeaponMontages[EWeaponState::PrimaryAttack];

	UAnimInstance* AnimInstance = GetAnimInstance();

	if (false == IsValid(AnimInstance))
	{
		return;
	}

	if (false == AnimInstance->Montage_IsPlaying(ComboMontage))
	{
		return;
	}

	SetNextComboEnable(MaxSequenceCount);
	
	if (true == GetNextComboCommand())
	{
		FName NextSectionName = *FString::Printf(TEXT("Combo_%d"), GetNextComboIndex());
		const int32 NextSectionIndex = ComboMontage->GetSectionIndex(NextSectionName);

		//AnimInstance->Montage_JumpToSection(NextSectionName, ComboMontage);
		Multicast_JumpToNextPrimaryAttackCombo(NextSectionIndex);
	}
}

void ADSCharacter_Boy::Multicast_JumpToNextPrimaryAttackCombo_Implementation(int32 NextSectionIndex)
{
	if (false == WeaponMontages.Contains(EWeaponState::PrimaryAttack))
	{
		return;
	}

	UAnimMontage* ComboMontage = WeaponMontages[EWeaponState::PrimaryAttack];

	UAnimInstance* AnimInstance = GetAnimInstance();

	if (false == IsValid(AnimInstance))
	{
		return;
	}

	if (false == AnimInstance->Montage_IsPlaying(ComboMontage))
	{
		AnimInstance->Montage_Play(ComboMontage);
	}

	const FName NextSectionName = ComboMontage->GetSectionName(NextSectionIndex);
	AnimInstance->Montage_JumpToSection(NextSectionName, ComboMontage);
}

void ADSCharacter_Boy::ProgressCombo(const ESkillType& SkillTag, int32 MaxSequenceCount)
{
	Super::ProgressCombo(SkillTag, MaxSequenceCount);

	if (SkillTag == ESkillType::PrimarySkill)
	{
		JumpToNextPrimaryAttackCombo(MaxSequenceCount);
	}
}
