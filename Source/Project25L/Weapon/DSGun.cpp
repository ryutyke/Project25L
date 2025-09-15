// Default
#include "Weapon/DSGun.h"

// UE
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "Components/CapsuleComponent.h"

// Game
#include "Character/DSArmedCharacter.h"
#include "Components/DSStatComponent.h"
#include "GameData/DSWeaponData.h"
#include "GameData/Physics/DSCollision.h"
#include "System/DSGameDataSubsystem.h"
#include "System/DSGameUtils.h"
#include "Gimmick/Skill/DSPortalActor.h"


ADSGun::ADSGun()
	: Super()
	, GravityGunTargets()
	, SpreadCoef(0.f)
	, ShotsFired(0)
	, FilePath(TEXT("SpreadOffsetFile.txt"))
{
	Mesh = CreateDefaultSubobject< USkeletalMeshComponent>(TEXT("Mesh"));
}

void ADSGun::BeginPlay()
{
	Super::BeginPlay();

	ACharacter* Character = Cast<ACharacter>(GetOwner());
	
	if (IsValid(Character))
	{
		if (Character->IsLocallyControlled())
		{
			// OnRep 함수에서 해주도록 변경 해야 함.
			//상대 경로가 아닌 절대 경로로 지정해주어야 한다. => 로컬에서 지정해준다.
			FString Path = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("SaveGames/") + FilePath);
			UDSGameUtils::LoadSpreadOffset(Path, SpreadCoef);
		}
	}
}

void ADSGun::InitializeData()
{
	Super::InitializeData();

	if (HasAuthority())
	{
		UDSGameDataSubsystem* DataManager = UDSGameDataSubsystem::Get(this);

		check(DataManager);

		FDSWeaponData* WeaponData = DataManager->GetDataRowByEnum<FDSWeaponData, EWeaponType>(EDataTableType::WeaponData, WeaponType);

		if (nullptr != WeaponData)
		{
			SpreadCoef = WeaponData->SpreadCoef;
		}
	}

	/*Recoil Curve Timeline 연결*/
	YawRecoil = UDSGameDataSubsystem::StreamableManager.LoadSynchronous(YawRecoilClass);
	PitchRecoil = UDSGameDataSubsystem::StreamableManager.LoadSynchronous(PitchRecoilClass);
}


FVector ADSGun::GetMuzzleSocketLocation() const
{
	return  Mesh->GetSocketLocation(TEXT("MuzzleFlush"));
}

FVector ADSGun::GetFinalDestination(const float& AimAngle, const float& SkillDistance) const
{
	if (false == IsValid(Mesh))
	{
		return FVector();
	}
	//시작 위치
	const FVector& StartLoc = GetMuzzleSocketLocation();
	// 0.f 가까우면 플레이어가 바라보는 방향으로, 스킬 Attack Distance 만큼 전달, 가장 기본적인 공격 위치 지정
	FVector TargetLoc = StartLoc + Mesh->GetRightVector() * SkillDistance;
	FVector TargetDir = (TargetLoc - StartLoc).GetSafeNormal();

	if (bUseAutoTargeting)
	{
		// 시각적 표현, 나이아가라
		// 목표지점까지는 나오고 => 사정거리 내에 적이 있는지 확인하고, 가까운 적한테 쏜다.
		// 오토 타겟팅을 설정한다면, 고려해서 위치를 반영해준다.
		TargetLoc = GetAutoTargetingLocation(AimAngle, StartLoc, TargetLoc, SkillDistance);

		//TargetLoc 으로 자연스럽게 회전한다. Turn To Target
		TargetDir = (TargetLoc - StartLoc).GetSafeNormal(); //Auto Targeting 결과에 대해서 회전 값을 다시 계산한다.
		FVector LookVector = TargetDir;

		//플레이어 자동으로 회전한다. 
	}

	//Yaw와 Pitch에 대한 결과값을 ServerRPC에게 전송해야한다.
	const FVector& SpreadTargetLoc = StartLoc + GetSpreadShotTargetLocation(TargetDir) * SkillDistance;
	return SpreadTargetLoc;
}


bool ADSGun::TryInstallInRange(FHitResult& HitResult, ECollisionChannel CollisionChannel, const float& AimAngle, const float& SkillDistance)
{
	UWorld* World = GetWorld();
	check(World);

	ADSArmedCharacter* Character = Cast<ADSArmedCharacter>(GetOwner());

	if (false == IsValid(Character))
	{
		return false;
	}

	const FVector& StartLoc = GetMuzzleSocketLocation();
	const FVector& TargetLocation = GetFinalDestination(AimAngle, SkillDistance);

	//라인을 쏜다.
	//라인을 쐈을 때 해당 채널에 대해 충돌이 발생했는지를 확인한다.
	bool bResult = false;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(GunLineTrace), false, Character); //식별자 
	FCollisionResponseParams ResponseParams;

	bResult = World->LineTraceSingleByChannel(
		HitResult,
		StartLoc,
		TargetLocation,
		CollisionChannel,
		Params,
		ResponseParams
	);

	//성공한 영역을 반환한다.
	return bResult;
}

FVector ADSGun::GetSpreadShotTargetLocation(const FVector& TargetDir) const
{
	UWorld* World = GetWorld();

	check(World);

	// Timeline의 가장 큰 Time을 가지고와서
	float MinTime, MaxTime;
	YawRecoil->GetTimeRange(MinTime, MaxTime);

	// 실제 누르는 횟수를 반영
	ShotsFired++;

	// 시간초를 반영해서
	float TimeSinceLastShot = FMath::Fmod(ShotsFired * InputThreshold, MaxTime);

	// Value값을 가져온다.
	float YawRecoilValue = YawRecoil->GetFloatValue(TimeSinceLastShot);
	float PitchRecoilValue = PitchRecoil->GetFloatValue(TimeSinceLastShot);

	const FRotator& RecoilRotator = FRotator(PitchRecoilValue, YawRecoilValue, 0.f);

	float Yaw = UDSGameUtils::GetSpreadOffset();
	float Pitch = UDSGameUtils::GetSpreadOffset();

	const FRotator& SpreadOffsetRotator = FRotator(Pitch, Yaw, 0.f) + RecoilRotator;
	const FRotator& BaseRot = TargetDir.Rotation();

	//짐벌락 현상을 없애기 위해 Quternion으로 계산
	const FQuat& AQuat = FQuat(SpreadOffsetRotator);
	const FQuat& BQuat = FQuat(BaseRot);

	// 두 곱셈을 통해서 Yaw/Pitch 값 반영
	FRotator FinalRotator = FRotator(BQuat * AQuat);;

	// 최종 방향 벡터
	FVector FinalDirection = FinalRotator.Vector();

	return FinalDirection;
}


// -------------------------------------------------------------------------- PrimarySkill -------------------------------------------------------------------------- //

void ADSGun::AttackPrimarySkill(const float& AutoAimAngle, const float& AttackDistance, const float& SkillDamage) const
{
	ADSArmedCharacter* Character = Cast<ADSArmedCharacter>(GetOwner());

	if (false == IsValid(Character))
	{
		return;
	}

	FVector StartLoc = GetMuzzleSocketLocation();
	FVector TargetLoc = GetFinalDestination(AutoAimAngle, AttackDistance);

	//애니메이션을 실행한다.
	Character->PlayAnimation(EWeaponState::Attack);

	//로컬이 서버인 경우 == 리슨서버는 아래 RPC를 실행하지 않는다.
	ServerRPC_PrimaryAttack(StartLoc, TargetLoc, SkillDamage);
}

void ADSGun::ServerRPC_PrimaryAttack_Implementation(const FVector& StartLoc, const FVector& TargetLoc, const float& SkillDamage) const
{
	ADSArmedCharacter* Character = Cast<ADSArmedCharacter>(GetOwner());

	if (false == IsValid(Character))
	{
		return;
	}

	// 실제 라인트레이스 진행
	UWorld* World = GetWorld();

	check(World);

	bool bResult = false;
	FHitResult OutHitResult;

	FCollisionQueryParams Params(SCENE_QUERY_STAT(GunLineTrace), false, Character); //식별자 
	FCollisionResponseParams ResponseParams;

	bResult = World->LineTraceSingleByChannel(
		OutHitResult,
		StartLoc,
		TargetLoc,
		ECollisionChannel::ECC_Pawn, 
		Params,
		ResponseParams
		);

	if (bResult)
	{
		ADSCharacterBase* TargetCharacter = Cast<ADSCharacterBase>(OutHitResult.GetActor());

		if (IsValid(TargetCharacter))
		{
			FDSDamageEvent DamageEvent;
			DamageEvent.DamageType = EDamageType::BaseAttack;
			
			TargetCharacter->TakeDamage(SkillDamage, DamageEvent, Character->GetController(), Character);
		}

	}

	// 리슨서버 애니메이션 실행
	if (Character->HasAuthority())
	{
		Character->ServerRPC_PlayWeaponAction(EWeaponState::Attack);
	}
}
// ----------------------------------------------------------------------------------------------------------------------------------------------------



// -------------------------------------------------------------------------- Skill1 -------------------------------------------------------------------------- //
void ADSGun::AttackSkill1(const TSoftObjectPtr<UNiagaraSystem>& NiagaraSystem, const FVector& TargetLoc, const float& AttackRadius, const float& Impulse, const float& SkillDamage, const float& Duration)
{

	DrawDebugCircle(
		GetWorld(),
		TargetLoc,                // 중심 좌표 (FVector)
		AttackRadius,            // 반지름 (float)
		32,                    // 세그먼트 수 (원 매끄럽게 할수록 높임)
		FColor::Cyan,          // 색상
		false,                 // 영구 표시 여부
		5.0f,                  // 지속 시간
		0,                     // 깊이 우선 순위
		1.0f,                  // 선 두께
		FVector(1, 0, 0),        // Y축 회전 기준 벡터 (보통 X축)
		FVector(0, 1, 0),        // Z축 회전 기준 벡터 (보통 Y축)
		false                  // 3D 회전 원 여부 (false면 평면 원)
	);

	ServerRPC_Skill1Attack(NiagaraSystem, TargetLoc, AttackRadius, Impulse, SkillDamage, Duration);
}

void ADSGun::ServerRPC_Skill1Attack_Implementation(const TSoftObjectPtr<UNiagaraSystem>& NiagaraSystem, const FVector& TargetLoc, const float& SkillDistance, const float& ZImpulse, const float& SkillDamage, const float& Duration)
{
	//애니메이션 및 연출 작업을 위한 Multicast
	MulticastRPC_Skill1Attack(NiagaraSystem, TargetLoc, Duration);
	
	ADSArmedCharacter* Character = Cast<ADSArmedCharacter>(GetOwner());

	if (false == IsValid(Character))
	{
		return;
	}

	UWorld* World = GetWorld();

	check(World);

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	TArray<AActor*> IgnoreActors;
	TArray<AActor*> OutActors;

	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn)); /*아이템 채널로 변경 해야 함!!*/
	IgnoreActors.Add(Character); //같은 팀은 제외해야한다.!!!

	//오버랩을 생성한다.
	bool Result = UKismetSystemLibrary::SphereOverlapActors(World, TargetLoc, SkillDistance, ObjectTypes, nullptr, IgnoreActors, OutActors);

	//해당 오버랩에 들어있는 적에게 중력을 끄고, AddForce 적용한다.
	if (Result)
	{
		for (AActor* Target : OutActors)
		{
			//나중에 팀플은 제외한다.!!!
			ADSCharacter* TargetCharacter = Cast<ADSCharacter>(Target);

			if (IsValid(TargetCharacter))
			{
				UCharacterMovementComponent* CharacterMovement = TargetCharacter->GetCharacterMovement();

				if (IsValid(CharacterMovement))
				{
					CharacterMovement->SetMovementMode(EMovementMode::MOVE_Flying);
					CharacterMovement->AddImpulse(FVector(0.0f, 0.f, ZImpulse), true); //현재 ZImpulse 값이 끝나면, 바로 멈춤. 
					CharacterMovement->AirControl = 0.1f; // 공중에서 약간의 제어만 가능, 0: 공중에서 방향전환 안됨 / 1: 공중에서도 방향을 자유롭게 바꿀 수 있음.
					CharacterMovement->BrakingDecelerationFlying = 100.0f; // 비행 중일 때 감속하는 정도로, 입력을 멈췄을 때 관성없이 빠르게 속도가 줄어들길 원하면 이 값이 높음.
					GravityGunTargets.Add(Target);
				}
				/*해당 타겟에게 데미지를 입힌다 => 도트 데미지인지 확인해야함.*/
				if (IsValid(TargetCharacter))
				{
					FDSDamageEvent DamageEvent;
					DamageEvent.DamageType = EDamageType::BaseAttack;

					TargetCharacter->TakeDamage(SkillDamage, DamageEvent, Character->GetController(), Character);
				}
			}
		}
	}

}

void ADSGun::MulticastRPC_Skill1Attack_Implementation(const TSoftObjectPtr<UNiagaraSystem>& NiagaraSystem, const FVector& TargetLocation,const float& Duration)
{
	UWorld* World = GetWorld();
	check(World);
	
	UNiagaraComponent* GravityNiagara = UNiagaraFunctionLibrary::SpawnSystemAtLocation(World, NiagaraSystem.LoadSynchronous(), TargetLocation, FRotator::ZeroRotator, FVector(2.0f, 2.0f, 2.f), false);

	FTimerHandle ValidTimer;
	World->GetTimerManager().SetTimer(ValidTimer, FTimerDelegate::CreateLambda([WeakObj = TWeakObjectPtr<UNiagaraComponent>(GravityNiagara), WeakThis = TWeakObjectPtr<ADSGun>(this)]
		{
			if (WeakObj.IsValid())
			{
				WeakObj->DestroyComponent();
			}

			if (WeakThis.IsValid())
			{
				//현재 클래스가 서버라면,
				if (WeakThis->HasAuthority())
				{
					WeakThis->EndAttackSkill1(); //중력 적용
				}
			}
		}), Duration, false); /*Duration*/
}

void ADSGun::EndAttackSkill1()
{

	for (const auto Target : GravityGunTargets)
	{
		//나중에 팀플은 제외한다.!!!
		ACharacter* TargetCharacter = Cast<ACharacter>(Target);

		if (IsValid(TargetCharacter))
		{
			UCharacterMovementComponent* CharacterMovement = TargetCharacter->GetCharacterMovement();

			if (IsValid(CharacterMovement))
			{
				CharacterMovement->SetMovementMode(EMovementMode::MOVE_Falling);
				CharacterMovement->GravityScale = 2.5f;
			}
		}
	}
}
// ----------------------------------------------------------------------------------------------------------------------------------------------------



// -------------------------------------------------------------------------- Skill2 -------------------------------------------------------------------------- //

bool ADSGun::CanPlacePortalAtLocation(const FVector& TargetLocation, ADSArmedCharacter* Character)
{
	UWorld* World = GetWorld();

	check(World);

	UCapsuleComponent* CapsuleComponent = Character->GetCapsuleComponent();
	if(false == IsValid(CapsuleComponent))
	{
		return false;
	}
	
	// 캐릭터의 캡슐컴포넌트의 Radius와 HalfHeight를 가지고 온다.
	const float Radius = CapsuleComponent->GetScaledCapsuleRadius();
	const float HalfHeight = CapsuleComponent->GetUnscaledCapsuleHalfHeight() - 10.0f;

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	TArray<AActor*> IgnoreActors;
	TArray<AActor*> OutActors;
	// Overlap되는 액터 타입
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic)); 
	IgnoreActors.Add(Character); 

	// 디버깅용
	DrawDebugCapsule(World, TargetLocation, HalfHeight, Radius, FQuat::Identity, FColor::Red, false, 5.0f, 3.0f);

	bool Result = UKismetSystemLibrary::CapsuleOverlapActors(World, TargetLocation, Radius, HalfHeight, ObjectTypes, nullptr, IgnoreActors, OutActors);

	if(false == Result)
	{
		return false;
	}

	int32 InstallableActorCount = 0; 
	for (const auto& OutActor : OutActors)
	{
		if (IsValid(OutActor))
		{
			UPrimitiveComponent* PrimitiveComponent = Cast<UPrimitiveComponent>(OutActor->GetRootComponent());
			if (IsValid(PrimitiveComponent))
			{
				// CCHANNEL_DSINSTALLPORTAL 채널과 Block으로 반응하는 액터의 개수를 센다.
				ECollisionResponse Response = PrimitiveComponent->GetCollisionResponseToChannel(CCHANNEL_DSINSTALLPORTAL);
				if (Response == ECR_Block)
				{
					++InstallableActorCount;
				}
			}
		}
	}

	// 설치 가능한 액터의 개수가 1보다 크거나 포탈 설치 가능한 액터의 수가 0이라면
	if (InstallableActorCount > 1 || InstallableActorCount == 0)
	{
		return false;
	}

	return true;
}

void ADSGun::GetPortalSurfaceData(FVector &HitNormal, EDSPortalSurfaceType &PortalSurfaceType) const
{
	if (FMath::Abs(HitNormal.Z) > 0.9f)
	{
		if (HitNormal.Z > 0.f) 
		{
			PortalSurfaceType =  EDSPortalSurfaceType::Floor;
		}		
		else 
		{
			PortalSurfaceType =  EDSPortalSurfaceType::Ceiling;
		}		
	}
	else
	{
		PortalSurfaceType = EDSPortalSurfaceType::Wall;
		HitNormal = -HitNormal;
	}
}

ADSPortalActor* ADSGun::TryPlacePortalEffect(const FPortalInstallationParams &PortalInstallationParams, const float& AimAngle, const float& SkillDistance, FVector& ExitLocation)
{
	UWorld* World = GetWorld();
	check(World);

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	const FVector StartLocation = GetMuzzleSocketLocation();
	const FVector EndLocation = GetFinalDestination(AimAngle, SkillDistance);

	bool bHit = TryInstallInRange(Hit, CCHANNEL_DSINSTALLPORTAL, AimAngle, SkillDistance);
	
	if (false == bHit)
	{
		return nullptr;
	}

	FVector TargetLocation = Hit.Location;

	DrawDebugLine( World, StartLocation, TargetLocation, FColor::Green, false, 5.0f, 0, 2.0f);

	ADSArmedCharacter* Character = Cast<ADSArmedCharacter>(GetOwner());

	if(false == IsValid(Character))
	{
		return nullptr;
	}

	if(false == CanPlacePortalAtLocation(TargetLocation, Character))
	{
		return nullptr;
	}

	FVector ActorNormal = Hit.Normal;
	EDSPortalSurfaceType SurfaceType = EDSPortalSurfaceType::Floor;

	GetPortalSurfaceData(ActorNormal,SurfaceType );

	// ExitLocation은 화이트홀(출구)에서 사용한다.
	ExitLocation = TargetLocation - (ActorNormal * PortalInstallationParams.InstallationOffset);

	const FRotator PortalRotation = FRotationMatrix::MakeFromXZ(ActorNormal, FVector::UpVector).Rotator();
	const FRotator FinalRotation = (PortalRotation.Quaternion()).Rotator();

	DrawDebugCoordinateSystem(GetWorld(), Hit.Location, PortalRotation, 100.0f, false, 5.0f);

	if (HasAuthority())
	{
		// 서버에서는 직접 Portal을 생성해야 한다.
		return InternalSpawnPortal(PortalInstallationParams.PortalClass, SurfaceType, TargetLocation, PortalRotation);
	}
	else
	{
		// 클라이언트는 서버로 생성 요청을 보내야 한다.
		ServerRPC_Skill2Attack(PortalInstallationParams.PortalClass, SurfaceType, TargetLocation, PortalRotation);
	}

	return nullptr;
}


void ADSGun::ServerRPC_Skill2Attack_Implementation(TSubclassOf<ADSPortalActor> PortalActorClass, EDSPortalSurfaceType SurfaceType, const FVector& TargetLocation, const FRotator& TargetRotation)
{
	ADSPortalActor* Portal = InternalSpawnPortal(PortalActorClass, SurfaceType, TargetLocation, TargetRotation);
	Portal->OnPortalSpawned();
}

ADSPortalActor* ADSGun::InternalSpawnPortal(TSubclassOf<ADSPortalActor> PortalActorClass, EDSPortalSurfaceType SurfaceType, const FVector& TargetLocation, const FRotator& TargetRotation)
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = GetOwner();
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	ADSPortalActor* Portal = GetWorld()->SpawnActor<ADSPortalActor>(
		PortalActorClass,
		TargetLocation,
		TargetRotation, //Yaw값이 누워있다. Roll을 이동
		SpawnParams
	);

	Portal->SetPortalSurfaceType(SurfaceType);
	return Portal;
}


// ----------------------------------------------------------------------------------------------------------------------------------------------------

void ADSGun::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADSGun, SpreadCoef);
}