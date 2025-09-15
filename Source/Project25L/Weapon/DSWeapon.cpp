// Default
#include "Weapon/DSWeapon.h"

// UE
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"

// Game
#include "System/DSGameDataSubsystem.h"
#include "GameData/Skill/DSSkillAttribute.h"
#include "Player/DSPlayerController.h"
#include "System/DSGameUtils.h"

ADSWeapon::ADSWeapon()
	: Super()
	, InputThreshold(0.f)
{
	bReplicates = true;
	SetReplicateMovement(true);
}

void ADSWeapon::BeginPlay()
{
	Super::BeginPlay();

	InitializeData();
}

FVector ADSWeapon::GetAutoTargetingLocation(const float& AimAngle, const FVector& StartLocation, const FVector& TargetLocation, const float& AttackRadius) const
{
	
	ACharacter* Character = Cast<ACharacter>(GetOwner());

	if (false == IsValid(Owner))
	{
		return FVector();
	}

	if (false == IsValid(Mesh))
	{
		return FVector();
	}

	FVector TargetLoc = TargetLocation;

	if (false == FMath::IsNearlyZero(AimAngle))
	{
		UWorld* World = GetWorld();
		
		check(World);

		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
		TArray<AActor*> IgnoreActors;
		TArray<AActor*> OutActors;

		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
	
		//캐릭터와 같은 팀일 경우 Ignore 시킬 예정
		IgnoreActors.Add(Character);
		
		bool Result = UKismetSystemLibrary::SphereOverlapActors(World, StartLocation, AttackRadius, ObjectTypes, nullptr, IgnoreActors, OutActors);
		
		DrawDebugCircle(
			GetWorld(),
			StartLocation,                // 중심 좌표 (FVector)
			AttackRadius,            // 반지름 (float)
			32,                    // 세그먼트 수 (원 매끄럽게 할수록 높임)
			FColor::Blue,          // 색상
			false,                 // 영구 표시 여부
			5.0f,                  // 지속 시간
			0,                     // 깊이 우선 순위
			1.0f,                  // 선 두께
			FVector(1, 0, 0),        // Y축 회전 기준 벡터 (보통 X축)
			FVector(0, 1, 0),        // Z축 회전 기준 벡터 (보통 Y축)
			false                  // 3D 회전 원 여부 (false면 평면 원)
		);

		if (Result)
		{
			// Overlap에서 나온 모든 적 NPC를 가지고 온다
				// 적이 시야범위에 드는지 확인한다.	
					//시야범위에 있는 적 중에서 가장 가까운 적을 목표지점으로 삼는다.
			float MinDistance = AttackRadius; //갈 수 있는 최대 거리임.

			for (const AActor* OutActor : OutActors)
			{

				bool bIsInFOV = UDSGameUtils::IsWithinCharacterFOV(Character, OutActor, AimAngle);

				if (bIsInFOV)
				{
					const FVector& OtherLocation = OutActor->GetActorLocation();

					float Distance = FMath::Abs(FVector::Distance(StartLocation, OtherLocation));

					if (Distance <= MinDistance)
					{
						TargetLoc = OtherLocation;
						MinDistance = Distance;
					}
				}
			}
		}
	}
	return TargetLoc;
}
void ADSWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADSWeapon, InputThreshold);
}

void ADSWeapon::InitializeData()
{
}

FVector ADSWeapon::GetFinalDestination(const float& AimAngle, const float& SkillDistance) const
{
	return FVector();
}

void ADSWeapon::PreAttackHitCheck(UAnimMontage* Anim)
{
	CurrentAttackAnim = Anim;
}

void ADSWeapon::AttackPrimarySkill(const float& AutoAimAngle, const float& AttackDistance, const float& SkillDamage) const
{
}

void ADSWeapon::EndPrimaryAttack() const
{
}

void ADSWeapon::EndAttackSkill1()
{
}

