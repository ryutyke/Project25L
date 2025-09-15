#pragma once
//Default
#include "CoreMinimal.h"

//UE
#include "Components/PawnComponent.h"

//Game
#include "Skill/Base/DSSkillSpec.h"
#include "Skill/Base/DSSkillActorInfo.h"
#include "Player/DSCheatManager.h"
#include "GameData/GameplayTag/DSGameplayTags.h"

//UHT
#include "DSSkillControlComponent.generated.h"



static bool bReplicateSkillsToSimulatedProxies = false;

// Delegate
DECLARE_MULTICAST_DELEGATE_OneParam(FOnSkillPressed, const FGameplayTag&);

USTRUCT()
struct FDSSkillSpecInitData 
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "DSSettings | Class")
	TSubclassOf<UDSSkillBase> SkillClass;
	
	UPROPERTY(EditAnywhere, Category = "DSSettings | Type")
	ESkillType SkillType;
	
	UPROPERTY(EditAnywhere, Category = "DSSettings | Class")
	FGameplayTag SkillTag;
};

UCLASS()
class PROJECT25L_API UDSSkillControlComponent : public UPawnComponent
{
	GENERATED_BODY()
	friend class UDSCheatManager;

public:
	UDSSkillControlComponent(const FObjectInitializer& ObjectInitializer);

	// 캐릭터 타입을 전달하면, 관련된 데이터 테이블 타입을 전달합니다.
	static EDataTableType GetSkillDataTableTypeByCharacter(ECharacterType CharacterType,int32 NonCharacterID);

	virtual void OnPlayerControllerSet() { }

	virtual void InitializeSkillSpecs();

	// SkillInputEvents
	TMap<FGameplayTag, FOnSkillPressed> OnSkillPressedEvents;
	TMap<FGameplayTag, FDSSkillSpecHandle> HasSkills;


	// --------------------------------------
	//	The important functions
	// --------------------------------------
	virtual void ReadyForReplication() override;

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	FDSSkillSpecHandle AddSkill(const FDSSkillSpec& Spec);
	virtual bool TryActivateSkill(FDSSkillSpecHandle AbilityToActivate, bool bAllowRemoteActivation = true);

	/** Will be called from AddSkill or from OnRep. Initializes events (inputs) with the add skill */
	virtual void OnAddSkill(FDSSkillSpec& SkillSpec);

	/** Will be called from RemoveSkill or from OnRep. Unbinds inputs with the given skill */
	virtual void OnRemoveSkill(FDSSkillSpec& SkillSpec);
	/**
 * Removes the specified skill.
 * This will be ignored if the actor is not authoritative.
 *
 * @param Handle Skill Spec Handle of the skill we want to remove
 */
	void ClearSkill(const FDSSkillSpecHandle& Handle);

	// ----------------------------------------------------------------------------------------------------------------
	// Skill Cancelling/Interrupts
	// ----------------------------------------------------------------------------------------------------------------

	/** Cancels the specified skill CDO. */
	void CancelSkill(UDSSkillBase* Skill);

	/** Cancels the skill indicated by passed in spec handle. If handle is not found among reactivated abilities nothing happens. */
	void CancelSkillHandle(const FDSSkillSpecHandle& SkillHandle);

	/** Cancels all abilities regardless of tags. Will not cancel the ignore instance */
	void CancelAllSkills(UDSSkillBase* Ignore = nullptr);


	// ----------------------------------------------------------------------------------------------------------------
	// Call notify callbacks above
	// ----------------------------------------------------------------------------------------------------------------

	/** Called from the skill to let the component know it is commit */
	virtual void NotifySkillCommit(UDSSkillBase* Skill);
	/** Called from the skill to let the component know it is Activated */
	virtual void NotifySkillActivated(const FDSSkillSpecHandle Handle, UDSSkillBase* Skill);
	/** Called from the skill to let the component know it is Failed */
	virtual void NotifySkillFailed(const FDSSkillSpecHandle Handle, UDSSkillBase* Skill);
	/** Called from the skill to let the component know it is ended */
	virtual void NotifySkillEnded(FDSSkillSpecHandle Handle, UDSSkillBase* Skill, bool bWasCancelled);

	virtual void ReplicateEndOrCancelSkill(FDSSkillSpecHandle Handle, UDSSkillBase* Skill, bool bWasCanceled);


	// ----------------------------------------------------------------------------------------------------------------
	//  FindSkillSpec
	// ----------------------------------------------------------------------------------------------------------------

	/** Returns an skill spec corresponding to given skill class. If modifying call MarkSkillSpecDirty */
	FDSSkillSpec* FindSkillSpecFromClass(TSubclassOf<UDSSkillBase> InSkillClass) const;

	/** Returns an skill spec from a handle. If modifying call MarkSkillSpecDirty */
	FDSSkillSpec* FindSkillSpecFromInputID(int32 InputID) const;

	/** Returns an skill spec from a handle. If modifying call MarkSkillSpecDirty */
	FDSSkillSpec* FindSkillSpecFromHandle(FDSSkillSpecHandle Handle) const;
	
	FDSSkillSpec* FindSkillSpecFromTag(FGameplayTag Tag) const;
	// ----------------------------------------------------------------------------------------------------------------
	//	Actor interaction
	// ----------------------------------------------------------------------------------------------------------------	
	virtual void InitSkillActorInfo(AActor* InOwnerActor, AActor* InAvatarActor);

	void RefreshSkillActorInfo();

	virtual void ClearActorInfo();

	UFUNCTION()
	virtual void ClearAllSkills();

	void SetOwnerActor(AActor* NewOwnerActor);
	AActor* GetOwnerActor() const { return SkillOwnerActor; }

	void SetAvatarActor_Direct(AActor* NewAvatarActor);
	AActor* GetAvatarActor_Direct() const { return SkillAvatarActor; }

	void SetAvatarActor(AActor* InAvatarActor);
	AActor* GetAvatarActor() const;

	UFUNCTION()
	void OnAvatarActorDestroyed(AActor* InActor);

	UFUNCTION()
	void OnOwnerActorDestroyed(AActor* InActor);


	/** Returns the list of all activatable Skills. Read-only. */
	const TArray<FDSSkillSpec>& GetActivatableSkills() const
	{
		return ActivatableSkills.Items;
	}

	/** Returns the list of all activatable Skills. */
	TArray<FDSSkillSpec>& GetActivatableSkills()
	{
		return ActivatableSkills.Items;
	}

	virtual void TryActivateSkillByTag(const FGameplayTag& SkillTag);
protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void InitializeComponent() override;
	virtual void OnRegister() override;
	virtual void PreNetReceive() override;
	virtual void BeginPlay() override;
	virtual void OnComponentDestroyed(bool bDestroyingHierarchy) override;
	virtual void OnUnregister() override;
	

protected:
	/** Cancel a specific skill spec */
	/* 스킬이 실행 전 호출되어야 작동한다. (Active 전)*/
	virtual void CancelSkillSpec(FDSSkillSpec& Spec, UDSSkillBase* Ignore);

	UFUNCTION(Client, reliable)
	void ClientRPC_EndSkill(FDSSkillSpecHandle SkillToEnd); 

	UFUNCTION(Server, reliable, WithValidation)
	void ServerRPC_CancelSkill(FDSSkillSpecHandle SkillToCancel);

	UFUNCTION(Client, reliable)
	void ClientRPC_CancelSkill(FDSSkillSpecHandle SkillToCancel);

	void CallServerTryActivateSkill(FDSSkillSpecHandle AbilityToActivate, bool InputPressed); 
	void CallServerEndSkill(FDSSkillSpecHandle SkillToEnd);

	// ----------------------------------------------------------------------------------------------------------------
	// Functions meant to be called from skill and subclasses, but not meant for general use
	// ----------------------------------------------------------------------------------------------------------------





	UFUNCTION(Server, reliable, WithValidation)
	void	ServerRPC_TryActivateSkill(FDSSkillSpecHandle SkillToActivate, bool InputPressed);

	UFUNCTION(Client, reliable)
	void	ClientRPC_TryActivateSkill(FDSSkillSpecHandle SkillToActivate);

	UFUNCTION(Client, Reliable)
	void	ClientRPC_ActivateSkillSucceed(FDSSkillSpecHandle SkillToActivate);

	UFUNCTION(Server, reliable, WithValidation)
	void	ServerRPC_EndSkill(FDSSkillSpecHandle SkillToEnd);

	UFUNCTION(Client, Reliable)
	void	ClientRPC_ActivateSkillFailed(FDSSkillSpecHandle SkillToActivate);

	UFUNCTION()
	void OnRep_ActivateSkills();

	/**
	 * 복제 대상 스킬 인스턴스를 내부 목록에 추가하고,
	 * NetSerialization을 위해 서브 오브젝트로 등록한다.
	 */
	void AddReplicatedInstancedSkill(UDSSkillBase* Skill);

	void RemoveReplicatedInstancedSkill(UDSSkillBase* Skill);

	void	RemoteEndOrCancelSkill(FDSSkillSpecHandle SkillToEnd,bool bWasCanceled);

	virtual void InternalServerTryActivateSkill(FDSSkillSpecHandle AbilityToActivate, bool InputPressed);


private:

	/**
	* 스킬 클래스(CDO)를 기반으로 새로운 인스턴스를 생성한다.
	* - Owner는 해당 SkillControlComponent가 붙은 액터
	* - Skill은 반드시 CDO여야 하며(HasAllFlags), GetClass()를 기반으로 생성
	* - 생성된 스킬 인스턴스는 Spec에 저장되며, GC 되지 않도록 리스트에 추가됨
	*/
	virtual UDSSkillBase* CreateNewInstanceOfSkill(FDSSkillSpec& Spec, const UDSSkillBase* Skill);

	/** Call to mark that an skill spec has been modified */
	void MarkSkillSpecDirty(FDSSkillSpec& Spec, bool WasAddOrRemove = false);

	bool InternalTryActivateSkill(FDSSkillSpecHandle Handle, UDSSkillBase** OutInstancedAbility = nullptr);

	UFUNCTION()
	void OnRep_SkillOwnerActor();

	bool ShouldTick(UDSSkillBase* SkillToTick) const;

public:
	FName AffectedAnimInstanceTag;
	TSharedPtr<FDSSkillActorInfo>	SkillActorInfo;
	bool IsSkillInputBlocked(int32 InputID) const;
	const TArray<uint8>& GetBlockedSkillBindings() const;
	FGameplayTagContainer GetActiveSkillTags() const;

protected:
	/** A pending activation that cannot be activated yet, will be rechecked at a later point */
	struct FPendingSkillInfo
	{
		bool operator==(const FPendingSkillInfo& Other) const
		{
			// Don't compare event data, not valid to have multiple activations in flight with same key and handle but different event data
			return Handle == Other.Handle;
		}

		/** Properties of the skill that needs to be activated */
		FDSSkillSpecHandle Handle;

		/** True if this skill was activated remotely and needs to follow up, false if the skill hasn't been activated at all yet */
		bool bPartiallyActivated;

		FPendingSkillInfo()
			: bPartiallyActivated(false)
		{}
	};

	/** 
	* 현재 실행중인 Skill들의 정보들 (복제되는 데이터)
	*/
	UPROPERTY(ReplicatedUsing = OnRep_ActivateSkills, BlueprintReadOnly, Transient, Category = "Skills")
	FDSSkillSpecContainer ActivatableSkills;

	UPROPERTY()
	TArray<TObjectPtr<UDSSkillBase>>	AllReplicatedInstancedSkills;
	TArray<TObjectPtr<UDSSkillBase>>& GetReplicatedInstancedSkills_Mutable() { return AllReplicatedInstancedSkills; }

	TArray<FPendingSkillInfo> PendingServerActivatedSkills;


	FTimerHandle OnRep_ActivateSkillsTimerHandle;

	UPROPERTY(Transient, Replicated)
	TArray<uint8> BlockedSkillBindings;

	uint8 bSkillPendingClearAll : 1;

	// ----------------------------------------------------------------------------------------------------------------
	// SkillDurationTimers;
	// ----------------------------------------------------------------------------------------------------------------
	TMap<FDSSkillSpecHandle, FTimerHandle> SkillDurationTimers;
	void ScheduleSkillEnd(FDSSkillSpecHandle Handle, float Duration, bool bReplicateEndSkill);

	// ----------------------------------------------------------------------------------------------------------------
	//	Clear Skill
	// ----------------------------------------------------------------------------------------------------------------	
	void CheckForClearedSkills();

	// ----------------------------------------------------------------------------------------------------------------
	//	Actor interaction
	// ----------------------------------------------------------------------------------------------------------------	
private:
	/** The actor that owns this component logically */
	UPROPERTY(ReplicatedUsing = OnRep_SkillOwnerActor)
	TObjectPtr<AActor> SkillOwnerActor;

	/** The actor that is the physical representation used for abilities. Can be NULL */
	UPROPERTY(ReplicatedUsing = OnRep_SkillOwnerActor)
	TObjectPtr<AActor> SkillAvatarActor;

	bool IsOwnerActorAuthoritative() const;

	virtual void DestroyActiveState();
	virtual void ForceCancelSkillDueToReplication(UDSSkillBase* Instance);
	
	UPROPERTY()
	uint8 bCachedIsNetSimulated : 1;

	UPROPERTY()
	uint8 bDestroyActiveStateInitiated : 1;
	
	UPROPERTY(EditAnywhere, Category = "DSSettings | Skill")
	TArray<FDSSkillSpecInitData> SkillInitializeDatas;
	
};
