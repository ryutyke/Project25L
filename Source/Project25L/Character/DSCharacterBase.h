#pragma once
//Default
#include "CoreMinimal.h"

//UE
#include "GameFramework/Character.h"

//Game
#include "GameData/DSEnums.h"
#include "Interface/DamageableInterface.h"

//UHT
#include "DSCharacterBase.generated.h"

class UDSSkillControlComponent;
class UDSStatComponent;
struct FDSDamageEvent;

UCLASS()
class PROJECT25L_API ADSCharacterBase : public ACharacter, public IDamageableInterface
{
	GENERATED_BODY()

public:
	ADSCharacterBase(const FObjectInitializer& ObjectInitializer);

	UAnimInstance* GetAnimInstance();
	UDSSkillControlComponent* GetSkillControlComponent() const { return SkillControlComponent; }
	UDSStatComponent* GetStatComponent() const { return StatComponent; }
public:
	virtual void ReceiveDamage_Implementation(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	
protected:
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_Controller() override;

protected:
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	virtual void InitSkillActorInfo();
	
protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DSSettings | Skill", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDSSkillControlComponent> SkillControlComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DSSettings | Stat", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDSStatComponent> StatComponent;

};
