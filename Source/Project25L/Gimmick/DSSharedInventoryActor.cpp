// Default
#include "Gimmick/DSSharedInventoryActor.h"

// Game
#include "DSLogChannels.h"

ADSSharedInventoryActor::ADSSharedInventoryActor()
{
	PrimaryActorTick.bCanEverTick = true;

	USceneComponent*	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	CubeMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CubeMeshComponent"));
	CubeMeshComponent->SetupAttachment(Root);
}

void ADSSharedInventoryActor::BeginPlay()
{
	Super::BeginPlay();
	
	// GetWorld()->GetTimerManager().SetTimer(
	// 	RotateTimerHandle,
	// 	this,
	// 	&ADSSharedInventoryActor::RotateCube,
	// 	RotateInterval,
	// 	true 
	// );
}

void ADSSharedInventoryActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const float RotateSpeedRad = 0.1f;
	const float DeltaAngle = RotateSpeedRad * DeltaTime;

	AddActorLocalRotation(FRotator(0.f, FMath::RadiansToDegrees(DeltaAngle), 0.f));

	if(false == IsValid(GetWorld())) return;
	if(false == IsValid(GetWorld()->GetCurrentLevel())) return;
	if(false == IsValid(GetWorld()->GetCurrentLevel())) return;
	DS_NETLOG(DSLog, Warning, TEXT("%s"), *(GetWorld()->GetCurrentLevel()->GetName()));
}

void ADSSharedInventoryActor::RotateCube()
{
	AddActorLocalRotation(FRotator(0.f, RotateValue, 0.f));
	if(false == IsValid(GetWorld())) return;
	if(false == IsValid(GetWorld()->GetCurrentLevel())) return;
	if(false == IsValid(GetWorld()->GetCurrentLevel())) return;
	DS_NETLOG(DSLog, Warning, TEXT("%s"), *(GetWorld()->GetCurrentLevel()->GetName()));
}

