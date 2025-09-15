#pragma once
//Default
#include "CoreMinimal.h"

//UE
#include "GameFramework/Character.h"

//Game
#include "GameData/DSEnums.h"

//UHT
#include "DSCharacterBase.generated.h"

class UDSSkillControlComponent;
class UDSStatComponent;
struct FDSDamageEvent;

UCLASS()
class PROJECT25L_API ADSCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	ADSCharacterBase(const FObjectInitializer& ObjectInitializer);

	UAnimInstance* GetAnimInstance();
	UDSSkillControlComponent* GetSkillControlComponent() const { return SkillControlComponent; }
	UDSStatComponent* GetStatComponent() const { return StatComponent; }
	
protected:
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_Controller() override;

public:
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

protected:
	virtual void InitSkillActorInfo();
	
protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DSSettings | Skill", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDSSkillControlComponent> SkillControlComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DSSettings | Stat", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDSStatComponent> StatComponent;

};
