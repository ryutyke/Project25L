// Default
#include "Skill/Base/DSSkillBase.h"

// UE
#include "Gameframework/characterMovementComponent.h"
#include "Gameframework/Character.h"

// Game
#include "Components/Skill/DSSkillControlComponent.h"
#include "DSLogChannels.h"
#include "GameData/DSEnums.h"

#include "System/DSGameDataSubsystem.h"
#include "GameData/Skill/DSSkillAttribute.h"
#include "Player/DSPlayerController.h"

UDSSkillBase::UDSSkillBase()
	: ReplicationPolicy(ESkillReplicationPolicy::ReplicateYes)
	, CurrentActorInfo(nullptr)
	, bSkillHasDuration(false)
	, bCanTick(false)
	, bIsActive(false)
	, TickInterval(0.5f)
	, ElapsedTime(0.f)
{
	StartWorldTime = -100.f;
	InstancingPolicy = ESkillInstancingPolicy::InstancedPerExecution;
	NetSecurityPolicy = ESkillNetSecurityPolicy::ClientOrServer;
	NetExecutionPolicy = ESkillNetExecutionPolicy::ServerInitiated;
	CooldownPolicy = ESkillCooldownPolicy::CooldownAfterActive;

}

UDSSkillBase::UDSSkillBase(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
	, ReplicationPolicy(ESkillReplicationPolicy::ReplicateYes)
	, CurrentActorInfo(nullptr)
	, bSkillHasDuration(false)
	, bCanTick(false)
	, bIsActive(false)
	, TickInterval(0.5f)
	, ElapsedTime(0.f)
{
	StartWorldTime = -100.f;
	InstancingPolicy = ESkillInstancingPolicy::InstancedPerExecution;
	NetSecurityPolicy = ESkillNetSecurityPolicy::ClientOrServer;
	NetExecutionPolicy = ESkillNetExecutionPolicy::ServerInitiated;
	CooldownPolicy = ESkillCooldownPolicy::CooldownAfterActive;

}

void UDSSkillBase::SetRemoteInstanceHasEnded()
{
}

void UDSSkillBase::BeginPlay()
{
	OnSkillInitialized();
}

void UDSSkillBase::Tick(float DeltaTime)
{
	ElapsedTime += DeltaTime;

	if (ElapsedTime >= TickInterval)
	{
		InternalTick(DeltaTime);

		ElapsedTime = 0.0f;
	}
}

void UDSSkillBase::NotifyAvatarDestroyed()
{
    OnSkillDeactivated();
}

// 스킬이 SkillControlComponent에 등록될 때 호출됨
void UDSSkillBase::OnAddSkill(const FDSSkillActorInfo* ActorInfo, ESkillType& InSkillType,  const FDSSkillSpec& Spec)
{
	SetCurrentActorInfo(Spec.Handle, InSkillType, ActorInfo);
	OnSkillInitialized();
}

void UDSSkillBase::OnRemoveSkill(const FDSSkillActorInfo* ActorInfo, const FDSSkillSpec& Spec)
{
	
}


void UDSSkillBase::PostNetInit()
{
	if (CurrentActorInfo == nullptr)
	{
		AActor* OwnerActor = Cast<AActor>(GetOuter());
		if (ensure(OwnerActor))
		{
			UDSSkillControlComponent* SkillControlComponent = OwnerActor->FindComponentByClass<UDSSkillControlComponent>();
			if (true == IsValid(SkillControlComponent))
			{
				CurrentActorInfo = SkillControlComponent->SkillActorInfo.Get();
			}
		}
	}
}

bool UDSSkillBase::IsInstantiated() const
{
	return !HasAllFlags(RF_ClassDefaultObject);
}

void UDSSkillBase::ConfirmActivateSucceed()
{
	if (false == HasAnyFlags(RF_ClassDefaultObject))
	{
		PostNetInit();
	}
}

void UDSSkillBase::SetCurrentActorInfo(const FDSSkillSpecHandle Handle, ESkillType& InType, const FDSSkillActorInfo* ActorInfo) 
{
	if(nullptr != ActorInfo)
	{
		CurrentActorInfo = ActorInfo;
	}
	CurrentSpecHandle = Handle;
	
	if(CurrentSkillType != InType)
	{
		InType = InType == ESkillType::None ? CurrentSkillType : InType;
	}
}

AActor* UDSSkillBase::GetSkillOwner() const
{
    const FDSSkillActorInfo* Info = GetCurrentActorInfo();
    return Info ? Info->SkillOwner.Get() : nullptr;
}

AActor* UDSSkillBase::GetSkillAvatar() const
{
    const FDSSkillActorInfo* Info = GetCurrentActorInfo();
    return Info ? Info->SkillAvatar.Get() : nullptr;
}

UDSSkillControlComponent* UDSSkillBase::GetSkillControlComponent() const
{
    return GetCurrentActorInfo() ? GetCurrentActorInfo()->SkillControlComponent.Get() : nullptr;
}


// -------------------------------------- ISkillLifeCycle --------------------------------------
void UDSSkillBase::OnSkillInitialized()
{
	const FDSSkillActorInfo *ActorInfo = GetCurrentActorInfo();

	if (nullptr == ActorInfo)
	{
		return;
	}
	
	APawn* Pawn = nullptr;
	ADSPlayerController* PlayerController = nullptr;

	if(false == IsValid(ActorInfo->SkillControlComponent.Get()))
	{
		return;
	}

	Pawn =  Cast<APawn>(ActorInfo->SkillControlComponent->GetOwner());

	if(IsValid(Pawn))
	{
		if(Pawn->GetController())
		{
			PlayerController = Cast<ADSPlayerController>(Pawn->GetController());
		}
	}

	if (IsValid(PlayerController))
	{
		ECharacterType CharacterType = PlayerController->GetCharacterType();

		//데이터 테이블을 사용해서, 현재 스킬에 대해 캐릭터의 AutoAimAngle 값을 가지고 온다.
		EDataTableType DataTableType = UDSSkillControlComponent::GetSkillDataTableTypeByCharacter(CharacterType, -1);

		UDSGameDataSubsystem* DataManager = UDSGameDataSubsystem::Get(PlayerController);
		check(DataManager);

		FDSSkillAttribute* SkillData = DataManager->GetDataRowByEnum<FDSSkillAttribute, ESkillType>(DataTableType, CurrentSkillType);

		if (nullptr != SkillData)
		{
			MaxCooltime = SkillData->MaxCooltime;
			AutoAimAngle = SkillData->AutoAimAngle;
			SkillRadius = SkillData->SkillRadius;
			SkillDuration = SkillData->SkillDuration;
			SkillDamage = SkillData->Damage;
			DodgeMaxSpeed = SkillData->MaxDodgeSpeed;
			DodgeMinSpeed = SkillData->MinDodgeSpeed;

			for (const auto& Elem : SkillData->Effects)
			{
				Effects.Add(Elem);
			}
		}
	}
}

void UDSSkillBase::OnSkillActivated()
{
}

void UDSSkillBase::OnSkillDeactivated()
{
}

void UDSSkillBase::PreActivate(const FDSSkillSpecHandle Handle, const FDSSkillActorInfo* ActorInfo)
{
	UDSSkillControlComponent* SkillControlComponent = ActorInfo->SkillControlComponent.Get();

	if(false == IsValid(SkillControlComponent))
	{
		return;
	}

	AActor* const MyActor = ActorInfo->SkillAvatar.Get();
	if (true == IsValid(MyActor) && false == ActorInfo->IsNetAuthority())
	{
		ACharacter* MyCharacter = Cast<ACharacter>(MyActor);
		if (true == IsValid(MyCharacter))
		{
			UCharacterMovementComponent* CharMoveComp = Cast<UCharacterMovementComponent>(MyCharacter->GetMovementComponent());

			if (CharMoveComp)
			{
				CharMoveComp->FlushServerMoves();
			}
		}
	}

	if (ESkillInstancingPolicy::NonInstanced != GetInstancingPolicy())
	{
		bIsActive = true;
		bIsBlockingOtherSkills = true;
		bIsCancelable = true;
	}

	RemoteInstanceEnded = false;

	SetCurrentActorInfo(Handle, CurrentSkillType, ActorInfo);

	SkillControlComponent->NotifySkillActivated(Handle, this);

	FDSSkillSpec* Spec = SkillControlComponent->FindSkillSpecFromHandle(Handle);

	if (nullptr == Spec)
	{
		return ;
	}

	// 함께 실행됐을 때 강제로 취소하는 스킬이 실행중이라면 해당 스킬을 취소한다.
	FGameplayTagContainer ActiveTags = SkillControlComponent->GetActiveSkillTags();

	for (const FGameplayTag& ActiveTag : ActiveTags)
	{
		if (Spec->CancelIfActiveTags.HasTag(ActiveTag))
		{
			FDSSkillSpec* CancelSpec = SkillControlComponent->FindSkillSpecFromTag(ActiveTag);
			if(nullptr != CancelSpec)
			{
				SkillControlComponent->CancelSkillHandle(CancelSpec->Handle);
			}
		}
	}

	if (LIKELY(Spec->ActiveCount < UINT8_MAX))
	{
		Spec->ActiveCount++;
	}
	else
	{
		DS_LOG(DSSkillLog, Warning, TEXT("PreActivate %s called when the Spec->ActiveCount (%d) >= UINT8_MAX"), *GetName(), (int32)Spec->ActiveCount);
	}
}

void UDSSkillBase::CallActivateSkill(const FDSSkillSpecHandle Handle, const FDSSkillActorInfo* ActorInfo )
{
	PreActivate(Handle, ActorInfo);
	ActivateSkill(Handle, ActorInfo);
}

void UDSSkillBase::ActivateSkill(const FDSSkillSpecHandle Handle, const FDSSkillActorInfo* ActorInfo)
{
	CommitSkill(Handle, ActorInfo);
}

bool UDSSkillBase::CommitSkill(const FDSSkillSpecHandle Handle, const FDSSkillActorInfo* ActorInfo)
{
	if(false == CommitCheck(Handle, ActorInfo))
	{
		return false; 
	}

	if(ESkillCooldownPolicy::CooldownAfterActive == CooldownPolicy)
	{
		ApplyCooldown(ActorInfo);
	}

	ApplyCost(ActorInfo);

	ActorInfo->SkillControlComponent->NotifySkillCommit(this);

	OnSkillActivated();

	return true;
}

void UDSSkillBase::CancelSkill(const FDSSkillSpecHandle Handle, const FDSSkillActorInfo* ActorInfo, bool bReplicateCancelSkill)
{
	if (true == CanBeCanceled())
	{
		if (true == bReplicateCancelSkill && nullptr != ActorInfo && true == ActorInfo->SkillControlComponent.IsValid())
		{
			ActorInfo->SkillControlComponent->ReplicateEndOrCancelSkill(Handle, this, true);
		}

		bool bReplicateEndSkill = false;
		bool bWasCancelled = true;

		EndSkill(Handle, ActorInfo, bReplicateEndSkill, bWasCancelled);
	}
}

bool UDSSkillBase::IsEndSkillValid(const FDSSkillSpecHandle Handle, const FDSSkillActorInfo* ActorInfo) const
{
	// 스킬이 이미 비활성화 상태이거나 스킬이 이미 종료 절차 중임(EndSkill() 진입 중 or 완료됨) + 이 스킬은 인스턴싱 되는 타입일 때
	if ((false == bIsActive || true == bIsSkillEnding) && ESkillInstancingPolicy::NonInstanced != GetInstancingPolicy())
	{
		DS_LOG(DSSkillLog, Warning, TEXT("IsEndSkillValid returning false on Skill %s due to EndSkill being called multiple times"), *GetName());
		return false;
	}

	UDSSkillControlComponent* SkillControlComponent = ActorInfo ? ActorInfo->SkillControlComponent.Get() : nullptr;
	if (false == IsValid(SkillControlComponent))
	{
		DS_LOG(DSSkillLog, Warning, TEXT("IsEndSkillValid returning false on Skill %s due to SkillSystemComponent being invalid"), *GetName());
		return false;
	}

	const FDSSkillSpec* Spec = SkillControlComponent->FindSkillSpecFromHandle(Handle);
	const bool bIsSpecActive = Spec ? Spec->IsActive() : IsActive();

	if (false == bIsSpecActive)
	{
		DS_LOG(DSSkillLog, Warning, TEXT("IsEndSkillValid returning false on Skill %s due spec not being active"), *GetName());
		return false;
	}

	return true;
}

void UDSSkillBase::EndSkill(const FDSSkillSpecHandle Handle, const FDSSkillActorInfo* ActorInfo, bool  bReplicateEndSkill, bool bWasCancelled)
{
	if (false == IsEndSkillValid(Handle, ActorInfo))
	{
		return;
	}

	if (ESkillInstancingPolicy::NonInstanced != GetInstancingPolicy())
	{
		bIsSkillEnding = true;
	}

	if (false == bIsActive && ESkillInstancingPolicy::NonInstanced != GetInstancingPolicy())
	{
		return;
	}
	
	UWorld* MyWorld = GetWorld();
	if (true == IsValid(MyWorld))
	{
		//이 객체와 연관된 모든 지연 실행 액션을 제거한다.
		MyWorld->GetLatentActionManager().RemoveActionsForObject(this);
		// 이 객체에 의해 예약된 모든 타이머 제거
		MyWorld->GetTimerManager().ClearAllTimersForObject(this);
	}

	if(ESkillCooldownPolicy::CooldownAfterEnd == GetCooldownPolicy())
	{
		ApplyCooldown(ActorInfo);
	}

	if (ESkillInstancingPolicy::NonInstanced != GetInstancingPolicy())
	{
		bIsActive = false;
		bIsSkillEnding = false;
	}

	UDSSkillControlComponent* const SkillControlComponent = ActorInfo->SkillControlComponent.Get();
	if(true == IsValid(SkillControlComponent))
	{
		if (true == bReplicateEndSkill)
		{
			SkillControlComponent->ReplicateEndOrCancelSkill(Handle, this, false);
		}
		
		SkillControlComponent->NotifySkillEnded(Handle, this, bWasCancelled);
	}
}


bool UDSSkillBase::CanActivateSkill(const FDSSkillSpecHandle Handle, const FDSSkillActorInfo* ActorInfo)
{
	AActor* const AvatarActor = ActorInfo ? ActorInfo->SkillAvatar.Get() : nullptr;
	if (false == IsValid(AvatarActor) || false == ShouldActivateSkill(AvatarActor->GetLocalRole()))
	{
		return false;
	}

	UDSSkillControlComponent* const SkillControlComponent = ActorInfo->SkillControlComponent.Get();
	if (false == IsValid(SkillControlComponent))
	{
		return false;
	}

	FDSSkillSpec* Spec = SkillControlComponent->FindSkillSpecFromHandle(Handle);
	if (nullptr == Spec)
	{
		return false;
	}

	// 함께 실행되면 안되는 스킬이 실행중이라면 실행 취소
	FGameplayTagContainer ActiveTags = SkillControlComponent->GetActiveSkillTags();

	for (const FGameplayTag& ActiveTag : ActiveTags)
	{
		if (Spec->BlockIfActiveTags.HasTag(ActiveTag))
		{
			return false;
		}
	}

	// 스킬의 쿨다운이 아직 남아있는 경우
	if (false == CheckCooldown(Handle, ActorInfo))
	{
		DS_LOG(DSSkillLog, Warning, TEXT("%s: %s could not be activated due to Cooldown"), *GetNameSafe(ActorInfo->SkillOwner.Get()), *GetNameSafe(Spec->Skill));
		DS_LOG(DSSkillLog, Warning, TEXT("%s could not be activated due to Cooldown"), *GetNameSafe(Spec->Skill));
		return false;
	}

	// 리소스가 부족한 경우
	if(false == CheckCost(Handle,ActorInfo))
	{
		DS_LOG(DSSkillLog, Warning, TEXT("%s: %s could not be activated due to Cost"), *GetNameSafe(ActorInfo->SkillOwner.Get()), *GetNameSafe(Spec->Skill));
		DS_LOG(DSSkillLog, Warning, TEXT("%s could not be activated due to Cost"), *GetNameSafe(Spec->Skill));

		return false;
	}

	// 스킬의 인풋이 차단된 상태라면
	if (true == SkillControlComponent->IsSkillInputBlocked(Spec->InputID))
	{
		DS_LOG(DSSkillLog, Warning, TEXT("%s: %s could not be activated due to blocked input ID %d"), *GetNameSafe(ActorInfo->SkillOwner.Get()), *GetNameSafe(Spec->Skill));
		DS_LOG(DSSkillLog, Warning, TEXT("%s could not be activated due to Cost"), *GetNameSafe(Spec->Skill));
		return false;
	}

	return true;
}


bool UDSSkillBase::ShouldActivateSkill(ENetRole Role) const
{
	return Role != ROLE_SimulatedProxy &&
		(Role == ROLE_Authority || (NetSecurityPolicy != ESkillNetSecurityPolicy::ServerOnly && NetSecurityPolicy != ESkillNetSecurityPolicy::ServerOnlyExecution));	// Don't violate security policy if we're not the server
}

void UDSSkillBase::SetShouldBlockOtherSkills(bool bShouldBlockSkills)
{
	
	if (bIsActive && ESkillInstancingPolicy::NonInstanced != GetInstancingPolicy() && bShouldBlockSkills != bIsBlockingOtherSkills)
	{
		UDSSkillControlComponent* SkillControlComponent = CurrentActorInfo->SkillControlComponent.Get();
		if (true == IsValid(SkillControlComponent))
		{
			// to Do Something
			// Comp->ApplySkillBlockAndCancelTags(SkillTags, this, bIsBlockingOtherAbilities, BlockAbilitiesWithTag, false, CancelAbilitiesWithTag);
		}
	}
}

bool UDSSkillBase::CanBeCanceled() const
{
	// Non instanced 스킬들은 항상 취소가 가능하다.
	if (ESkillInstancingPolicy::NonInstanced == GetInstancingPolicy())
	{
		return true; 
	}

	return bIsCancelable;
}

void UDSSkillBase::SetCanBeCanceled(bool bCanBeCanceled)
{
	if (ESkillInstancingPolicy::NonInstanced == GetInstancingPolicy() && bCanBeCanceled != bIsCancelable)
	{
		bIsCancelable = bCanBeCanceled;
	}
}

bool UDSSkillBase::CommitCheck(const FDSSkillSpecHandle Handle, const FDSSkillActorInfo* ActorInfo)
{
	const bool bValidHandle = Handle.IsValid();
	const bool bValidActorInfoPieces = (ActorInfo && (ActorInfo->SkillControlComponent != nullptr));
	const bool bValidSpecFound = bValidActorInfoPieces && (ActorInfo->SkillControlComponent->FindSkillSpecFromHandle(Handle) != nullptr);

	// Spec이 유효하지 않으면 Commit 불가
	if (false == bValidHandle || false == bValidActorInfoPieces || false == bValidSpecFound)
	{
		DS_LOG(DSSkillLog, Warning, TEXT("CommitCheck provided an invalid handle or actor info or couldn't find ability spec: %s Handle Valid: %d ActorInfo Valid: %d Spec Not Found: %d"), *GetName(), bValidHandle, bValidActorInfoPieces, bValidSpecFound);
		return false;
	}

	// 스킬의 쿨다운이 아직 남아있는 경우
	if (false == CheckCooldown(Handle, ActorInfo))
	{
		return false;
	}

	// 리소스가 부족한 경우
	if (false == CheckCost(Handle, ActorInfo))
	{
		return false;
	}

	// 모든 조건 통과 시 Commit 가능
	return true;
}

bool UDSSkillBase::IsActive() const
{
	// InstancedPerActor 스킬만 여러 번의 발동 사이에서 상태를 유지함  
	if (ESkillInstancingPolicy::InstancedPerActor == GetInstancingPolicy())
	{
		return bIsActive;
	}

	// NonInstanced 스킬에서 이 함수를 호출하는 것은 잘못된 사용이며 대신 스킬 스펙(SkillSpec)에서 IsActive를 호출해야 함
	if (ESkillInstancingPolicy::NonInstanced == GetInstancingPolicy())
	{
		// DS_LOG(DSSkillLog, Warning, TEXT("UGameplayAbility::IsActive() called on %s NonInstanced ability, call IsActive on the Ability Spec instead"), *GetName());
	}

	// NonInstanced 및 InstancedPerExecution 스킬은  인스턴스가 살아있기만 하면 활성 상태로 간주됨
	return IsValid(this);
}


// -------------------------------------------------------------------------- Cost -------------------------------------------------------------------------- //
bool UDSSkillBase::CheckCost(const FDSSkillSpecHandle Handle, const FDSSkillActorInfo* ActorInfo) const
{
	return true;
}

void UDSSkillBase::ApplyCost(const FDSSkillActorInfo* ActorInfo)
{
}
// ----------------------------------------------------------------------------------------------------------------------------------------------------

 
// -------------------------------------------------------------------------- Cooldown -------------------------------------------------------------------------- //
void UDSSkillBase::GetCooldownTimeRemainingAndDuration(FDSSkillSpecHandle Handle, const FDSSkillActorInfo* ActorInfo, float& TimeRemaining, float& OutCooldownDuration) const
{
	if(CooldownPolicy == ESkillCooldownPolicy::None)
	{
		return;
	}

	TimeRemaining = 0.f;
	OutCooldownDuration = 0.f;

	UDSSkillControlComponent* const SkillControlComponent = ActorInfo->SkillControlComponent.Get();
	if (true == IsValid(SkillControlComponent))
	{
		UWorld* World = SkillControlComponent->GetWorld();
		if (IsValid(World))
		{
			const float CurrentTime = World->GetTimeSeconds();
			const float Elapsed = CurrentTime - StartWorldTime;
			if (Elapsed < MaxCooltime)
			{
				UE_LOG(LogTemp, Warning, TEXT("Cooldown already active. Skip ApplyCooldown. Remaining: %f"),
					MaxCooltime - (CurrentTime - StartWorldTime));
				return;
			}
			OutCooldownDuration = MaxCooltime;

			// 남은 쿨타임은 Duration에서 경과 시간을 뺀 값 (음수이면 0으로 처리)
			TimeRemaining = FMath::Max(OutCooldownDuration - Elapsed, 0.f);
		}
	}
}

bool UDSSkillBase::CheckCooldown(const FDSSkillSpecHandle Handle, const FDSSkillActorInfo* ActorInfo) const
{
	if(CooldownPolicy == ESkillCooldownPolicy::None)
	{
		return true;
	}

	// 스킬이 쿨타임을 가진다면, 현재 시간과 시작 시간의 차이를 계산
	UDSSkillControlComponent* const SkillControlComponent = ActorInfo->SkillControlComponent.Get();
	if (true == IsValid(SkillControlComponent))
	{
		UWorld* World = SkillControlComponent->GetWorld();
		if (IsValid(World))
		{
			float CurrentTime = World->GetTimeSeconds();
			float Elapsed = CurrentTime - StartWorldTime;

			// 아직 쿨타임이 남아 있다면 false를 반환
			if (Elapsed < MaxCooltime)
			{
				return false;
			}
		}
	}

	return true;
}

void UDSSkillBase::ApplyCooldown(const FDSSkillActorInfo* ActorInfo)
{
	if(CooldownPolicy == ESkillCooldownPolicy::None)
	{
		return ;
	}

	UDSSkillControlComponent* const SkillControlComponent = ActorInfo->SkillControlComponent.Get();
	if (true == IsValid(SkillControlComponent))
	{
		UWorld* World = SkillControlComponent->GetWorld();
		if (IsValid(World))
		{
			StartWorldTime = ActorInfo->SkillControlComponent->GetWorld()->GetTimeSeconds();
		}
	}
}
// --------------------------------------------------------------------------
