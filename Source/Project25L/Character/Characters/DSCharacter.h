#pragma once
//Default
#include "CoreMinimal.h"

//UE
#include "GameplayTagContainer.h"

//Game
#include "Character/DSCharacterBase.h"
#include "GameData/DSEnums.h"

//UHT
#include "DSCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UDSInventoryComponent;
class ADSGiftBox;
class UDSPlayerInputComponent;
class UDSFlightComponent;
class UDSMainWidget;
class APlayerState;
class UTimelineComponent;
class UDSCameraControlComponent;
class USphereComponent;

UCLASS()
class PROJECT25L_API ADSCharacter : public ADSCharacterBase
{
	GENERATED_BODY()
	
public:
	ADSCharacter(const FObjectInitializer& ObjectInitializer);

	DECLARE_MULTICAST_DELEGATE_TwoParams(FOnMovementModeChanged, bool, bool);
	FOnMovementModeChanged OnMovementModeChanged;

	DECLARE_MULTICAST_DELEGATE_OneParam(FOnSendItemFailed, EAlertStatus AlertStatus);
	FOnSendItemFailed OnSendItemFailed;

	virtual void OnRep_Controller() override;
/*Getter/Setter 함수*/
public:
	FORCEINLINE FVector CalPlayerLocalCameraStartPos();
	FORCEINLINE bool GetIsDodge() { return bIsDodge; }
	FORCEINLINE void SetIsDodge(float MinSpeed, float MaxSpeed);
	FORCEINLINE FVector GetCameraForwardVector();
	FORCEINLINE FVector GetCameraRightVector();
	FORCEINLINE float GetFOV() { return FOV; }
	FORCEINLINE UDSFlightComponent* GetFlightComponent() const { return FlightComponent; }
	FORCEINLINE UDSPlayerInputComponent* GetPlayerInputComponent() const { return DSPlayerInputComponent; }
	FORCEINLINE virtual float GetInputThreshold(FGameplayTag& SkillTag);
	FORCEINLINE UDSInventoryComponent* GetInventoryComponent() const { return InventoryComponent; }

	FORCEINLINE void InitializeComboCount() {CurrentComboIndex = 0;}

	UFUNCTION(Server, Reliable)
	void ServerRPC_SetNextComboCommand(bool bInHasNextComboCommand);
	
	FORCEINLINE void SetNextComboCommand(bool bInHasNextComboCommand) { bHasNextComboCommand = bInHasNextComboCommand; }

	FORCEINLINE bool GetNextComboCommand() const { return bHasNextComboCommand; }
	FORCEINLINE int GetReceivedComboCommandCnt() const { return ReceivedComboCommandCnt; }
	FORCEINLINE void DecreaseReceivedComboCommandCnt() { ReceivedComboCommandCnt--; }
	FORCEINLINE void InitReceivedComboCommandCnt() { ReceivedComboCommandCnt = 0; }
	FORCEINLINE void SetComboInputTiming(bool IsInputTiming) { bIsComboInputTiming = IsInputTiming; }
	FORCEINLINE bool IsComboInputTiming() { return bIsComboInputTiming; }
	FORCEINLINE void SetIsProgressCombo(bool InbIsProgressCombo) { bIsProgressCombo = InbIsProgressCombo; }
	FORCEINLINE bool IsProgressCombo() { return bIsProgressCombo; }

	void SetCanFly(bool InbCanFly);
	bool CanFly() { return bCanFly; }

	FORCEINLINE void SetNextComboEnable(int32 MaxSequenceCount) 
	{
		if (bHasNextComboCommand)
		{
			CurrentComboIndex = (CurrentComboIndex+1) % (MaxSequenceCount + 1);
		}
		else
		{
			CurrentComboIndex = 0;
		}
	}

	FORCEINLINE int GetNextComboIndex() { return CurrentComboIndex; }

	virtual void PreAttackHitCheck(UAnimMontage* Anim) {}
	virtual void AttackHitCheck(const FName& HitSocket, const ESkillType& SkillTag) { }
	virtual void AttackHitCheck(const ESkillType& SkillTag) {}
	virtual void PostAttackHitCheck() {}

	virtual void ProgressCombo(const ESkillType& SkillTag, int32 MaxSequenceCount) { }

public:
/*수류탄을 들고 있을 때 1인칭으로 변경되어져야 한다.*/
	UFUNCTION(Server, Reliable)
	void ServerRPC_EnableControllerRotationYaw(bool bEnable);

	// 실제 줌인과 관련되어 있는 델리게이트
	DECLARE_MULTICAST_DELEGATE(FOnZoomChanged);
	FOnZoomChanged OnZoomInStarted;
	FOnZoomChanged OnZoomOutStarted;

	/*
		카메라 모드 변환시 호출할 델리게이트
		CameraMode를 전달하고, Timeline 시작과 되돌림을 전달하는 bool 변수를 사용
	*/
	DECLARE_MULTICAST_DELEGATE_TwoParams(FOnCameraModeChanged, const ECameraMode&, bool);
	FOnCameraModeChanged OnCameraModeChanged;


protected:
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_EnableControllerRotationYaw(bool bEnable);

protected:
	/*현재 플레이어의 경사를 확인하기 위한 코드*/
	void SetTerrainInclineType(const ETerrainInclineType& InclineType);
protected:
	/*Dodge*/
	UFUNCTION(Server, Reliable)
	void ServerRPC_Dodge(float MinSpeed, float MaxSpeed);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_Dodge(float MinSpeed, float MaxSpeed);

	UFUNCTION()
	void DodgeInterp(float CurveValue);

	UFUNCTION()
	void OnDodgeFinished();
public:
	void InitializeUI();
protected:
	void InitializeStat();
	void BindWidget(UDSStatComponent* InStatComponent,ACharacter* InCharacter, UDSMainWidget* MainWidget, int32 PlayerIndex);
protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void PossessedBy(AController* NewController) override;
	virtual void Tick(float DeltaSeconds) override;
public:

	UFUNCTION(Server, Reliable)
	void ServerRPC_ReadyPlayer(bool bCanClose, int32 PlayerCount,const FGameplayTag& ReadyPlayerWidgetTag);

	UFUNCTION(Client, Reliable)
	void ClientRPC_ReadyPlayer(bool bCanClose, int32 PlayerCount, const FGameplayTag& ReadyPlayerWidgetTag);

public:
	/*Cheat*/
	UFUNCTION(Server, Unreliable)
	void ServerRPC_UseItem(int32 ItemID, int32 ItemCount);

	UFUNCTION(Server, Unreliable)
	void ServerRPC_PrintItem();

	bool StoreItem(int32 ItemID);
	void AddSkill(const int32 InputID);
	void PickupItem(AActor* Interactor);
	void SelectedItem(AActor* Interactor);
	void SendItem(int32 PlayerId, int32 ItemID);
public:
/*Item*/

	void TryPickupItem(int32 ItemIdx);
	/*각 로컬에서만 동작하는 아이템 획득 시도하는 함수*/
	void TryInteraction();
	
	
public:

	UFUNCTION(Server, Reliable)
	void ServerRPC_RemoveItemData(int32 IndexToRemove);

	UFUNCTION(NetMulticast, Reliable)
	void ClientRPC_RemoveItemData(int32 IndexToRemove);

protected:
	
	UFUNCTION(Server, Reliable)
	void ServerRPC_SetSurroundingItem(ADSGiftBox* Interactor);

	UFUNCTION(Server, Reliable)
	void ServerRPC_SendItme(int32 PlayerId, int32 ItemID);

	UFUNCTION(Client, Reliable)
	void ClientRPC_ItemSendFaild();
protected:
/*Input*/
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent);
	virtual void BeginPlay() override;
protected:

	UFUNCTION()
	void OnDetectComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnDetectComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:
	void SetJumpHeight(bool bIsRun);

	UFUNCTION(Server, Unreliable)
	void ServerRPC_SetJumpVelocity(bool bIsRun);

protected:
	void CheckSlopeState();

protected:
	/*경사와 관련된 함수*/
	UPROPERTY(Transient)
	ETerrainInclineType TerrainInclineType;
	
	UPROPERTY(EditAnywhere, Category = "DSSettings | Incline")
	float InclineCheckInterval;

	UPROPERTY(Transient)
	float ElapsedInclineTime;
protected:
	UPROPERTY(Transient)
	TSet<TWeakObjectPtr<ADSCharacter>> DetectedEnemies;
	/*캐릭터 상태에 들어가야할 변수*/
protected:
	UPROPERTY(Transient)
	uint8 bIsDodge : 1;

	UPROPERTY(Transient)
	TObjectPtr<UCurveFloat> DodgeCurve;
	
	UPROPERTY(Transient)
	float DodgeMinSpeed;

	UPROPERTY(Transient)
	float DodgeMaxSpeed;

	UPROPERTY(EditAnywhere, Category = "DSSettings | Dodge")
	TSoftObjectPtr<UCurveFloat> DodgeCurveClass;

	UPROPERTY(EditAnywhere, Category = "DSSettings | Dodge", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UTimelineComponent> DodgeTimelineComponent;
protected:
	UPROPERTY(Transient)
	uint8 bCanFly : 1;
protected:
	/*콤보 공격을 하는 모든 공격에서 체크할 것*/
	UPROPERTY(Transient)
	int32 CurrentComboIndex;
	
	UPROPERTY(Transient)
	uint8 bHasNextComboCommand : 1;

	// Not using on Client
	UPROPERTY(Transient)
	int32 ReceivedComboCommandCnt;

	// Always 0 on Client
	UPROPERTY(Transient)
	uint8 bIsComboInputTiming : 1;

	UPROPERTY(Transient)
	uint8 bIsProgressCombo : 1;

protected:
	UPROPERTY(Transient)
	TWeakObjectPtr<ADSGiftBox> HeldItem;

protected:
	UPROPERTY(Transient, Replicated)
	ECharacterType CharacterType;

protected:
	/*Character Setting*/

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DSSettings | Jump")
	float NomalJumpHeight = 35.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DSSettings | Jump")
	float RunJumpHeight = 40.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DSSettings | Farming")
	float FarmingRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DSSettings | FOV")
	float FOV;

protected:


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DSSettings | Inventory", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDSInventoryComponent> InventoryComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DSSettings | Flight", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDSFlightComponent> FlightComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DSSettings | Input", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDSPlayerInputComponent> DSPlayerInputComponent;

protected:
	/*Custom Components*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DSSettings | Inventory", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDSCameraControlComponent> CameraControlComponent;	

protected:
	/*Spring Arm, Camera*/

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DSSettings | Camera", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> CameraSpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DSSettings | Camera", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "DSSettings | Collision")
	TObjectPtr<USphereComponent> CameraTriggerSphere;
};
