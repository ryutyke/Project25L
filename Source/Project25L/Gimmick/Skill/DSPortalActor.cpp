// Default
#include "DSPortalActor.h"

// UE
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Character.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"

// Game
#include "Character/Characters/DSCharacter.h"
#include "GameData/Physics/DSCollision.h"
#include "GameData/GameplayTag/DSGameplayTags.h"
#include "Components/Skill/DSSkillControlComponent.h"
#include "DSLogChannels.h"

ADSPortalActor::ADSPortalActor()
{
	PrimaryActorTick.bCanEverTick = false;

    TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
    RootComponent = TriggerBox;
    TriggerBox->SetCollisionProfileName(TEXT("OverlapAllDynamic"));


    SphereMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SphereMesh"));
    SphereMeshComponent->SetupAttachment(RootComponent);
    SphereMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereAsset(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
    if (SphereAsset.Succeeded())
    {
        SphereMeshComponent->SetStaticMesh(SphereAsset.Object);
        SphereMeshComponent->SetRelativeScale3D(FVector(0.5f));
    }

    PortalEffectComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("PortalEffect"));
    PortalEffectComponent->SetupAttachment(SphereMeshComponent);

    bisPortalActive = false;

    TeleportDelayTime = 0.5f;
    EndSkillDelayTime = 1.f;
}

void ADSPortalActor::SetExitLocation(FVector InExitLocation)
{
    ExitLocation = InExitLocation;
}

void ADSPortalActor::SetPortalSurfaceType(EDSPortalSurfaceType InPortalSurfaceType)
{
    PortalSurfaceType = InPortalSurfaceType;
}

void ADSPortalActor::OnPortalSpawned()
{
    SphereMeshComponent->SetVisibility(true);
}

void ADSPortalActor::OnActivatePortal()
{
    ServerRPC_SpawnEffect();
}

void ADSPortalActor::OnResetPortal()
{
    ServerRPC_DeactivateEffect();
}

void ADSPortalActor::BeginPlay()
{
    Super::BeginPlay();

    TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ADSPortalActor::OnPortalOverlap);
}

void ADSPortalActor::ServerRPC_SpawnEffect_Implementation()
{
    bisPortalActive = true;
    SphereMeshComponent->SetVisibility(false);
    MulticastRPC_SpawnEffect();
}

void ADSPortalActor::MulticastRPC_SpawnEffect_Implementation()
{
    PortalEffectComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), Effect, GetActorLocation(), GetActorRotation() + FRotator(90.0f, 0.f, 0.f), FVector(2.0f, 2.0f, 2.f), false);
}

void ADSPortalActor::ServerRPC_DeactivateEffect_Implementation()
{
    MulticastRPC_DeactivateEffect();
    bisPortalActive = false;
    SphereMeshComponent->SetVisibility(false);
}

void ADSPortalActor::MulticastRPC_DeactivateEffect_Implementation()
{
    PortalEffectComponent->Deactivate();
    PortalEffectComponent->SetVisibility(false);
}

void ADSPortalActor::OnPortalOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    // 출구는 오버랩 되도 아무런 이벤트가 발생하지 않기 때문에 리턴시킨다.
	if(EDSPortalType::Exit == PortalType)
	{
		return;
	}

    if(false == bisPortalActive)
    {
        return;
    }

    ADSCharacter* Player = Cast<ADSCharacter>(OtherActor);

    if (false == IsValid(Player))
    {
        return;
    }

    if (false == IsValid(OtherComp))
    {
       return;
    }

    const ECollisionResponse Response = OtherComp->GetCollisionResponseToChannel(CCHANNEL_DSPLAYER);
    
    // 플레이어라면 이동 시킨다.
    if(Response == ECR_Block)
    {
        bisPortalActive = false;
        
        // 텔레포트
        GetWorld()->GetTimerManager().SetTimer(
        TeleportTimerHandle,
        FTimerDelegate::CreateLambda([Player, this]()
        {
            if (IsValid(Player))
            {
                if(PortalSurfaceType == EDSPortalSurfaceType::Wall)
                {
					const FVector ForwardVector = -GetActorForwardVector();
					Player->SetActorRotation(ForwardVector.Rotation());
                }
                Player->SetActorLocation(ExitLocation);
                DS_LOG(DSSkillLog, Log, TEXT("ExitLocation : %s"), *ExitLocation.ToString());
            }
        }),
        TeleportDelayTime, false);

		 // 스킬 취소
        GetWorld()->GetTimerManager().SetTimer(
        CancelSkillTimerHandle,
        FTimerDelegate::CreateLambda([this]()
        {
            ADSCharacter* Character = Cast<ADSCharacter>(GetOwner());
            if (false == IsValid(Character)) 
            {
                return;
            }

            UDSSkillControlComponent* SkillControlComponent = Character->GetSkillControlComponent();
            if (IsValid(SkillControlComponent))
            {
                FDSSkillSpec* SkillSpec = SkillControlComponent->FindSkillSpecFromTag(FDSTags::GetDSTags().Skill_Skill2);
                if (SkillSpec && SkillSpec->Handle.IsValid())
                {
                    SkillControlComponent->CancelSkillHandle(SkillSpec->Handle);
                }
            }
        }),
        EndSkillDelayTime, false);
    }
}
