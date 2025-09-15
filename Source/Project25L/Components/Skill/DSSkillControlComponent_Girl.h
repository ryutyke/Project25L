#pragma once

// Default
#include "CoreMinimal.h"

// Game
#include "Components/Skill/DSSkillControlComponent.h"

// UHT
#include "DSSkillControlComponent_Girl.generated.h"

 class ADSCharacter_Girl;

UCLASS()
class PROJECT25L_API UDSSkillControlComponent_Girl : public UDSSkillControlComponent
{
	GENERATED_BODY()

public:
	UDSSkillControlComponent_Girl(const FObjectInitializer& ObjectInitializer);

	virtual void InitializeComponent() override;
	virtual void InitSkillActorInfo(AActor* InOwnerActor, AActor* InAvatarActor) override;

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
