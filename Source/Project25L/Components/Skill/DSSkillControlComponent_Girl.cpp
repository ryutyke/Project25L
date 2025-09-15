
// Default
#include "Components/Skill/DSSkillControlComponent_Girl.h"

// UE
#include "EngineUtils.h"
#include "Net/UnrealNetwork.h"

// Game
#include "System/DSEventSystems.h"
#include "Character/DSArmedCharacter.h"
#include "DSLogChannels.h"

UDSSkillControlComponent_Girl::UDSSkillControlComponent_Girl(const FObjectInitializer& ObjectInitializer)
    :Super(ObjectInitializer)
{
}

void UDSSkillControlComponent_Girl::InitializeComponent()
{
    Super::InitializeComponent();

}

void UDSSkillControlComponent_Girl::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

}

void UDSSkillControlComponent_Girl::InitSkillActorInfo(AActor* InOwnerActor, AActor* InAvatarActor)
{
    Super::InitSkillActorInfo(InOwnerActor, InAvatarActor);

	SkillActorInfo->AnimInstance = Cast<ACharacter>(GetOwner())->GetMesh()->GetAnimInstance();

}
