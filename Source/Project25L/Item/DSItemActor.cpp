// Default
#include "Item/DSItemActor.h"

// UE
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StreamableManager.h"
#include "Net/UnrealNetwork.h"

// Game
#include "GameData/Items/DSItemData.h"
#include "Character/Characters/DSCharacter.h"
#include "DSLogChannels.h"
#include "System/DSGameUtils.h"
#include "System/DSEventSystems.h"

ADSItemActor::ADSItemActor()
	: Super()
	, StoredItems()
	, ItemID(0)
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));

	RootComponent = Trigger;

	Trigger = CreateDefaultSubobject<USphereComponent>(TEXT("Trigger"));

	Mesh->SetupAttachment(Trigger);
	Trigger->OnComponentBeginOverlap.AddDynamic(this, &ADSItemActor::OnComponentBeginOverlap);
	Trigger->OnComponentEndOverlap.AddDynamic(this, &ADSItemActor::OnComponentEndOverlap);

	bReplicates = true;
	SetReplicateMovement(true);

	PrimaryActorTick.bCanEverTick = true;
}

void ADSItemActor::InitializeItemData(TMap<int32, int32> ItemData)
{
	for (const auto& Item : ItemData)
	{
		for (int CntIdx = 0; CntIdx < Item.Value; CntIdx++)
		{
			StoredItems.Add({ Item.Key });
		}
	}
}

void ADSItemActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADSItemActor, StoredItems);
}

void ADSItemActor::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		InitLocation();
	}
}

void ADSItemActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bIsOverlapped)
	{
		if (IsValid(Mesh) == false)
		{
			return;
		}
		if (IsValid(OverlappedCharacter))
		{
			bool bIsInFOV = UDSGameUtils::IsWithinCharacterFOV(OverlappedCharacter, this, OverlappedCharacter->GetFOV());

			if (bIsInFOV)
			{
				//아이템에 아웃라인이 생긴다.
				if (IsValid(OutlineMaterial))
				{
					Mesh->SetOverlayMaterial(OutlineMaterial);
				}
			}
			else
			{
				//범위를 벗어나면 아이템에 아웃라인 제거
				if (IsValid(Mesh->OverlayMaterial))
				{
					Mesh->SetOverlayMaterial(nullptr);
				}
			}
		}
	}
}

void ADSItemActor::InitLocation()
{
	FCollisionQueryParams CollisionParams(SCENE_QUERY_STAT(GroundCheckLineTrace), false, this); //식별자 

	FHitResult OutHitResult;

	FVector StartLocation = GetActorLocation();

	//업벡터의 반대방향으로 라인트레이스를 쏜다.
	FVector EndLocation = StartLocation + 500.f * GetActorUpVector() * -1;
	bool bResult = GetWorld()->LineTraceSingleByChannel(
		OutHitResult,
		StartLocation,
		EndLocation,
		ECollisionChannel::ECC_Visibility,
		CollisionParams
	);

	if (bResult) //바닥이 있다면,
	{
		//메시가 묻히지 않게 z/2만큼 올려준다.
		SetActorLocation(OutHitResult.Location + 5.f); //해당 액터를 위치시킴
	}

	if (IsValid(RootComponent))
	{
		//더이상 액터를 이동시키지 않기 위해 Mobility를 변경해준다. -> 자원 낭비를 막기 위함
		RootComponent->Mobility = EComponentMobility::Stationary;
	}
}

void ADSItemActor::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
	//UI 띄우는 작업 ('F'키를 눌러라.. 등등)
	ADSCharacter* Character = Cast<ADSCharacter>(OtherActor);


	if (false == IsValid(Character))
	{
		return;
	}

	//잡몬한테 반응하면 안되기 때문에, ADSCharacter로 변경
	//NPC아닌지 확인 -> 틱 꺼야함
	// ADSCharacter* Character = Cast<ADSCharacter>(OtherActor); 
	if (IsValid(Character))
	{	
		
		bool bIsLocallyExecutable = UDSGameUtils::IsLocallyExecutable(Character);

		if (false == bIsLocallyExecutable)
		{
			return;
		}

		for (int i = 0; i < StoredItems.Num(); i++)
		{
			DS_LOG(DSItemLog, Log, TEXT("Item Id %d"), StoredItems[i].ID);
		}

		bIsOverlapped = true;
		OverlappedCharacter = Character;
	}
}

void ADSItemActor::OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

	ADSCharacter* Character = Cast<ADSCharacter>(OtherActor);
	if (false == IsValid(Character))
	{
		return;
	}
	/*
	문제점 : 아이템을 먹었을 경우, Overlap End되어지면서, 모든 아이템에서 경계선이 사라지는 문제가 있음.
	해결해야함!

	해결, 캐릭터일 때에만 반응하도록 변경
	*/
	//각 클라이언트 환경에서만 보이도록 한다.
	if (bIsOverlapped)
	{
		bIsOverlapped = false;
		OverlappedCharacter = nullptr;

		if (IsValid(Mesh))
		{
			if (IsValid(Mesh->OverlayMaterial))
			{
				Mesh->SetOverlayMaterial(nullptr);
			}
		}
	}

}
