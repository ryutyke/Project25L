//Default
#include "Character/Characters/DSCharacter.h"

//UE
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Components/TimelineComponent.h"
#include "EngineUtils.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameplayTagContainer.h"
#include "InputMappingContext.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"

//Game
#include "Components/DSCameraControlComponent.h"
#include "Components/DSCharacterMovementComponent.h"
#include "Components/DSFlightComponent.h"
#include "Components/DSInventoryComponent.h"
#include "Components/DSPlayerInputComponent.h"
#include "Components/DSStatComponent.h"
#include "Components/Skill/DSSkillControlComponent.h"
#include "DSLogChannels.h"
#include "Game/DSGameState.h"
#include "GameData/Skill/DSSkillAttribute.h"
#include "GameData/GameplayTag/DSGameplayTags.h"
#include "GameData/GameplayTag/DSGameplayTags.h"
#include "GameData/Items/DSItemData.h"
#include "GameData/Physics/DSCollision.h"
#include "HUD/DSHUD.h"
#include "Item/DSGiftBox.h"
#include "Item/DSItemActor.h"
#include "Player/DSPlayerController.h"
#include "Player/DSPlayerState.h"
#include "Skill/Base/DSTestSkill.h"
#include "System/DSEventSystems.h"
#include "System/DSGameDataSubsystem.h"
#include "System/DSGameUtils.h"
#include "System/DSUIManagerSubsystem.h"
#include "UI/Game/DSMainWidget.h"
#include "UI/Game/Player/DSHPBar.h"
#include "UI/Game/Player/DSPlayerInfo.h"
#include "UI/HUB/DSReadyPlayers.h"
#include "UI/Item/DSItemPickupSlot.h"


ADSCharacter::ADSCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UDSCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
	, TerrainInclineType(ETerrainInclineType::Flat)
	, InclineCheckInterval(0.3f)
	, ElapsedInclineTime(0.0f)
	, bIsDodge(false)
	, bCanFly(false)
	, CurrentComboIndex(0)
	, bHasNextComboCommand(false)
	, ReceivedComboCommandCnt(0)
	, bIsComboInputTiming(false)
	, bIsProgressCombo(false)
	, HeldItem(nullptr)
	, FarmingRadius(100.f)

{
	InventoryComponent = CreateDefaultSubobject<UDSInventoryComponent>(TEXT("InventoryComponent"));
	FlightComponent = CreateDefaultSubobject<UDSFlightComponent>(TEXT("FlightComponent"));
	CameraControlComponent = CreateDefaultSubobject<UDSCameraControlComponent>(TEXT("CameraControlComponent"));
	CameraTriggerSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CameraTriggerSphereComponent"));

	//Channel을 설정
	CameraTriggerSphere->SetupAttachment(RootComponent);
	CameraTriggerSphere->OnComponentBeginOverlap.AddDynamic(this, &ADSCharacter::OnDetectComponentBeginOverlap);
	CameraTriggerSphere->OnComponentEndOverlap.AddDynamic(this, &ADSCharacter::OnDetectComponentEndOverlap);

	CameraSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraSpringArm"));

	//Mesh 부착
	CameraSpringArm->SetupAttachment(GetMesh(), TEXT("spine_03"));
	CameraSpringArm->bUsePawnControlRotation = false; //QuaterView는 회전하지 않는다.
	CameraSpringArm->bEnableCameraLag = true;
	CameraSpringArm->TargetArmLength = 500.f;
	CameraSpringArm->SetUsingAbsoluteRotation(true);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(CameraSpringArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;
	CameraSpringArm->bUsePawnControlRotation = true;

	GetCapsuleComponent()->SetCollisionObjectType(CCHANNEL_DSPLAYER);


	DodgeTimelineComponent = CreateDefaultSubobject<UTimelineComponent>(TEXT("DodgeTimelineComponent"));

	SetJumpHeight(false);
	
}

void ADSCharacter::SetTerrainInclineType(const ETerrainInclineType& InclineType)
{
	if (false == IsValid(CameraControlComponent))
	{
		return;
	}

	if (TerrainInclineType != InclineType)
	{
		//카메라 모드로 변환
		//TerrainInclineType은 제거하고, InclineType을 카메라 모드에 추가한다.
		CameraControlComponent->ApplyCameraModeByIncline(TerrainInclineType, false);
		TerrainInclineType = InclineType;
		CameraControlComponent->ApplyCameraModeByIncline(InclineType, true);
	}
}

FVector ADSCharacter::CalPlayerLocalCameraStartPos()
{
	return Camera->GetComponentLocation() + GetCameraForwardVector() * CameraSpringArm->TargetArmLength;
}

FVector ADSCharacter::GetCameraForwardVector()
{
	return Camera->GetForwardVector();
}

FVector ADSCharacter::GetCameraRightVector()
{
	return Camera->GetRightVector();
}

void ADSCharacter::SetCanFly(bool InbCanFly)
{
	bCanFly = InbCanFly;
	DSEVENT_DELEGATE_INVOKE(OnMovementModeChanged, bCanFly, true);
}

void ADSCharacter::OnRep_Controller()
{
	Super::OnRep_Controller();

	if (IsValid(DSPlayerInputComponent))
	{
		DSPlayerInputComponent->SetupInputComponent(InputComponent);
	}
}

void ADSCharacter::SetIsDodge(float MinSpeed, float MaxSpeed)
{
	ServerRPC_Dodge(MinSpeed, MaxSpeed);
}

float ADSCharacter::GetInputThreshold(FGameplayTag& SkillTag)
{
	if (false == IsValid(SkillControlComponent))
	{
		return 0.f;
	}

	if (false == FDSTags::GetDSTags().SkillType.Contains(SkillTag))
	{
		return 0.f;
	}

	UDSGameDataSubsystem* DataManager = UDSGameDataSubsystem::Get(this);

	check(DataManager);
	
	ESkillType SkillType = FDSTags::GetDSTags().SkillType[SkillTag];

	EDataTableType DataTableType = SkillControlComponent->GetSkillDataTableTypeByCharacter(CharacterType, -1);

	FDSSkillAttribute* InputData = DataManager->GetDataRowByEnum<FDSSkillAttribute, ESkillType>(DataTableType, SkillType);

	if (nullptr != InputData)
	{
		DS_LOG(DSSkillLog, Log, TEXT("%s %lf"), *SkillTag.ToString(), InputData->InputThreshold);
		return InputData->InputThreshold;
	}

	return 0.2f;
}

void ADSCharacter::ServerRPC_SetNextComboCommand_Implementation(bool bInHasNextComboCommand)
{
	if (true == bInHasNextComboCommand)
	{
		ReceivedComboCommandCnt++;
	}
}

void ADSCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	InitializeStat();
}

void ADSCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (IsLocallyControlled())
	{
		ElapsedInclineTime += DeltaSeconds;
		if (InclineCheckInterval < ElapsedInclineTime)
		{
			//경사로를 확인한다.
			CheckSlopeState();
			ElapsedInclineTime = 0.f;
		}
	}
}

void ADSCharacter::CheckSlopeState()
{
	USkeletalMeshComponent* LocalMesh = GetMesh();
	if (false == IsValid(LocalMesh))
	{
		return;
	}

	UWorld* World = GetWorld();
	check(World);

	FVector Velocity = GetVelocity();

	// 라인 체크를 한다.
	// 라인과의 Normal Vector 구한다. => 노멀 벡터와 Velocity간의 내적을 수행
	FCollisionQueryParams Params(SCENE_QUERY_STAT(TerrainInclineTrace), false, this); //식별자 
	FCollisionResponseParams ResponseParams;

	FVector StartLocation = LocalMesh->GetSocketLocation(TEXT("ik_foot_root"));
	FVector TargetLocation = StartLocation + (-LocalMesh->GetUpVector() * 10.f);

	FHitResult HitResult;

	bool bResult = World->LineTraceSingleByChannel(
		HitResult,
		StartLocation,
		TargetLocation,
		CCHANNEL_DSINCLINE,
		Params,
		ResponseParams
	);

	const float Alpha = 15.f;

	if (bResult)
	{
		const FVector& NormalVector = HitResult.Normal;

		const float& AngleDegree = UDSGameUtils::GetSeparationAngle(Velocity, NormalVector);
		
		if (AngleDegree >= 90.f + Alpha) // 90 + a => 105도 이상
		{
			//상승
			SetTerrainInclineType(ETerrainInclineType::UpHill);
		}
		else if(AngleDegree <= 90.f - Alpha) // 90 - a => 75도 이하
		{
			//하강
			SetTerrainInclineType(ETerrainInclineType::DownHill);
		}
		else
		{
			//평지
			SetTerrainInclineType(ETerrainInclineType::Flat);
		}
	}
}

void ADSCharacter::InitializeStat()
{
	ADSPlayerState* LocalPlayerState = GetPlayerState<ADSPlayerState>();

	if (IsValid(LocalPlayerState))
	{
		CharacterType = LocalPlayerState->GetCharacterType();

		StatComponent->InitializeStats(EDataTableType::CharacterData, CharacterType, -1);
	}
}

void ADSCharacter::ServerRPC_Dodge_Implementation(float MinSpeed, float MaxSpeed)
{
	MulticastRPC_Dodge(MinSpeed, MaxSpeed);
}

void ADSCharacter::MulticastRPC_Dodge_Implementation(float MinSpeed, float MaxSpeed)
{
	// 리플리케이션을 사용하면, 클라이언트 - 서버 모두 동기화가 불가능함
	// 동시에 동기화 할 수 있도록 RPC에서 bool 변수 트리거
	bIsDodge = !bIsDodge;

	DodgeMinSpeed = MinSpeed;
	DodgeMaxSpeed = MaxSpeed;

	if (IsValid(DodgeTimelineComponent))
	{
		DodgeTimelineComponent->PlayFromStart();
	}
}

void ADSCharacter::DodgeInterp(float CurveValue)
{
	FVector ForwardVector = GetActorForwardVector();
	
	float DodgeCurrentSpeed = FMath::Lerp(DodgeMinSpeed, DodgeMaxSpeed, CurveValue);

	FVector DodgeVelocity = ForwardVector * DodgeCurrentSpeed;

	GetCharacterMovement()->Velocity = DodgeVelocity;	
}

void ADSCharacter::ServerRPC_EnableControllerRotationYaw_Implementation(bool bEnable)
{
	MulticastRPC_EnableControllerRotationYaw(bEnable);
}

void ADSCharacter::MulticastRPC_EnableControllerRotationYaw_Implementation(bool bEnable)
{
	//클라이언트 환경에서 모두 true되어야함.
	//카메라가 바라보는 방향으로 따라가야하기 때문임.
	UCharacterMovementComponent* LocalCharacterMovement = GetCharacterMovement();

	if (false == IsValid(LocalCharacterMovement))
	{
		return;
	}

	if (bEnable)
	{
		bUseControllerRotationYaw = true;
		LocalCharacterMovement->bOrientRotationToMovement = false; //캐릭터나 액터가 이동할 때 그 방향으로 회전을 수행(마우스에 따라서 회전을 수행하도록 도와줌)
		//애니메이션 블루프린트도 8방향으로 변경하든가 해야할 듯? => or 그냥 bool로 분기하든가..
	}
	else
	{
		bUseControllerRotationYaw = false;
		LocalCharacterMovement->bOrientRotationToMovement = true; //캐릭터나 액터가 이동할 때 그 방향으로 회전을 수행(마우스에 따라서 회전을 수행하도록 도와줌)
		//애니메이션 블루프린트도 8방향으로 변경하든가 해야할 듯? => or 그냥 bool로 분기하든가..
	}

}

void ADSCharacter::OnDodgeFinished()
{
	bIsDodge = false;
	DodgeMinSpeed = 0.f;
	DodgeMaxSpeed = 0.f;
}

void ADSCharacter::InitializeUI()
{
	if (false == IsLocallyControlled())
	{
		return;
	}

	//현재 로컬 환경에서
	//Pawn을 모두 가져온다.
	UDSUIManagerSubsystem* UIManager = UDSUIManagerSubsystem::Get(this);

	check(UIManager);

	UDSMainWidget* MainWidget = Cast<UDSMainWidget>(UIManager->GetTopLayer(FDSTags::GetDSTags().UI_Layer_Persistent));

	if (false == IsValid(MainWidget))
	{
		return;
	}
	
	//자기 자신 초기화
	BindWidget(StatComponent, this, MainWidget, 0);

	UWorld* World = GetWorld();

	check(World);

	AGameStateBase* GameState = World->GetGameState();

	int32 PlayerIndex = 1;
	for (APlayerState* PS : GameState->PlayerArray)
	{
		ADSCharacter* Character = PS->GetPawn<ADSCharacter>();

		if (this == Character)
		{
			//나 자신과 같은 경우는 이미 연결했기 때문에 제외한다.
			continue;
		}
		if (IsValid(Character))
		{
			UDSStatComponent* TargetStatComponent = Character->StatComponent;
			if (IsValid(TargetStatComponent))
			{
				BindWidget(TargetStatComponent, Character, MainWidget, PlayerIndex++);
			}
		}
	}
}

void ADSCharacter::BindWidget(UDSStatComponent* InStatComponent, ACharacter* Character, UDSMainWidget* MainWidget, int32 PlayerIndex)
{
	if (false == IsValid(InStatComponent) || false == IsValid(MainWidget))
	{
		return;
	}

	UDSHPBar* HPBar = MainWidget->GetHPBar(PlayerIndex);
	UDSPlayerInfo* PlayerInfo = MainWidget->GetPlayerInfo(PlayerIndex);
	if (IsValid(HPBar) && IsValid(PlayerInfo))
	{
		if (IsValid(InStatComponent))
		{
			/*초기화 => StatComponent가 이미 초기화 된 이후에 이 함수가 호출되어진다.*/
			DSEVENT_DELEGATE_BIND(InStatComponent->OnHPChanged, HPBar, &UDSHPBar::SetHP);
			DSEVENT_DELEGATE_BIND(InStatComponent->OnHPChanged, PlayerInfo, &UDSPlayerInfo::SetText);

			//초기화 해준다.
			DSEVENT_DELEGATE_INVOKE(InStatComponent->OnHPChanged, InStatComponent->GetCurrentHP(), InStatComponent->GetMaxHP());
		}
	}

	if (IsValid(Character))
	{
		ADSPlayerState* LocalPlayerState = Character->GetPlayerState<ADSPlayerState>();

		if (IsValid(LocalPlayerState))
		{
			ECharacterType LocalCharacterType = LocalPlayerState->GetCharacterType();

			DSEVENT_DELEGATE_BIND(LocalPlayerState->OnCharacterTypeChanged, PlayerInfo, &UDSPlayerInfo::InitPlayerSetting);
			
			DSEVENT_DELEGATE_INVOKE(LocalPlayerState->OnCharacterTypeChanged, LocalCharacterType, FText::FromString(LocalPlayerState->GetPlayerName()));
		}
	}
}

void ADSCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ADSCharacter, CharacterType);

}


void ADSCharacter::ServerRPC_ReadyPlayer_Implementation(bool bCanClose, int32 PlayerCount,const FGameplayTag& ReadyPlayerWidgetTag)
{
	//모든 클라이언트를 가져온다.
	UWorld* World = GetWorld();
	check(World);

	for (auto Iterator = World->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* PlayerController = Iterator->Get();
		if (IsValid(PlayerController))
		{
			// 로직
			ADSCharacter* Character = Cast<ADSCharacter>(PlayerController->GetCharacter());

			if (IsValid(Character))
			{
				Character->ClientRPC_ReadyPlayer(bCanClose, PlayerCount, ReadyPlayerWidgetTag);
			}
		}
	}

}

void ADSCharacter::ClientRPC_ReadyPlayer_Implementation(bool bCanClose, int32 PlayerCount, const FGameplayTag& ReadyPlayerWidgetTag)
{
	UWorld* World = GetWorld();

	check(World);

	ADSGameState* GameState = Cast<ADSGameState>(World->GetGameState());

	if (false == IsValid(GameState))
	{
		return;
	}

	int32 MaxPlayerCount = GameState->GetMaxPlayerCount();

	UDSUIManagerSubsystem* UIManager = UDSUIManagerSubsystem::Get(this);
	check(UIManager);

	if (false == bCanClose)
	{
		UIManager->PushContentToLayer(ReadyPlayerWidgetTag);
	}
	else
	{
		//false로 변경되어 질때 UI를 없앤다.
		UIManager->PopContentToLayer(ReadyPlayerWidgetTag);
	}


	//플레이어가 n명일 때
	UDSReadyPlayers* WidgetLayer = Cast<UDSReadyPlayers>(UIManager->GetTopLayer(FDSTags::GetDSTags().UI_Layer_Modal));

	if (IsValid(WidgetLayer))
	{
		WidgetLayer->UpdatePlayerCount(PlayerCount, MaxPlayerCount);
	}

}

//Cheat 
void ADSCharacter::ServerRPC_UseItem_Implementation(int32 ItemID, int32 ItemCount)
{
	for (int ItemIdx = 0; ItemIdx < ItemCount; ItemIdx++)
	{
		if (IsValid(InputComponent))
		{
			InventoryComponent->OnItemUsed(ItemID);
		}
	}
}

void ADSCharacter::ServerRPC_PrintItem_Implementation()
{
	if (IsValid(InventoryComponent))
	{
		InventoryComponent->PrintItem();
	}
}

bool ADSCharacter::StoreItem(int32 ItemID)
{
	FDSItemInfo ItemInfo = {ItemID};
	return InventoryComponent->StoreReceivedItem(nullptr, ItemInfo);
}

void ADSCharacter::AddSkill(const int32 InputID)
{
	FDSSkillSpec NewSkillSpec(UDSTestSkill::StaticClass(), ESkillType::TestSkill, FDSTags::GetDSTags().Skill_TestSkill, InputID);
	GetSkillControlComponent()->AddSkill(NewSkillSpec);
}

void ADSCharacter::TryInteraction()
{
	UWorld* World = GetWorld();
	check(World);

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	TArray<AActor*> IgnoreActors;
	TArray<AActor*> OutActors;

	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Visibility)); /*아이템 채널로 변경 해야 함!!*/
	IgnoreActors.Add(this);

	FVector Location = GetActorLocation();

	bool Result = UKismetSystemLibrary::SphereOverlapActors(World, Location, FarmingRadius, ObjectTypes, nullptr, IgnoreActors, OutActors);
	float MinDistance = FLT_MAX;

	if (Result)
	{

		ADSItemActor* SurroundingItem = nullptr;
		for (AActor* Actor : OutActors)
		{
			//가장 가까운 아이템을 섭치한다.
			//시야 각도에 들어오는지 확인한다.
			ADSItemActor* TmpItem = Cast<ADSItemActor>(Actor);

			if (IsValid(TmpItem))
			{
				bool bIsInFOV = UDSGameUtils::IsWithinCharacterFOV(this, TmpItem, GetFOV());
				//시야각 범위 내에서
				if (bIsInFOV)
				{
					//거리가 가장 가까운 아이템을 줍는다.
					float Distance = FVector::Distance(Location, TmpItem->GetActorLocation());

					if (MinDistance > Distance)
					{
						MinDistance = Distance;
						SurroundingItem = TmpItem;
					}
				}
			}
		}

		if (IsValid(SurroundingItem))
		{
			//업데이트
			EInteractType InteractionType = SurroundingItem->GetInteractType();

			switch (InteractionType)
			{
			case EInteractType::PickupItem:
				PickupItem(SurroundingItem);
				break;
			case EInteractType::SelectedItem:
				SelectedItem(SurroundingItem);
				break;
			}
		}
	}
}


void ADSCharacter::ServerRPC_RemoveItemData_Implementation(int32 IndexToRemove)
{
	//모든 클라이언트를 가지고 와서, 현재 클라이언트의 HeldItem과 같을 경우에 제거해주어야한다.
	//클라이언트RPC를 쏴야함.

	if (false == HeldItem.IsValid())
	{
		return;
	}

	UWorld* World = GetWorld();
	check(World);

	for (FConstPlayerControllerIterator Iterator = World->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* PlayerController = Iterator->Get();

		if (IsValid(PlayerController))
		{
			ADSCharacter* Character = PlayerController->GetPawn<ADSCharacter>();

			if (IsValid(Character))
			{
				//현재 서버의 HeldItem과 같다면, UI를 반영해야함.

				if (Character->HeldItem.IsValid())
				{
					if (Character->HeldItem == HeldItem)
					{
						Character->ClientRPC_RemoveItemData(IndexToRemove);
					}
				}
			}
		}
	}

	// 서버에서 데이터 업데이트
	HeldItem->RemoveItemData(IndexToRemove);
}

void ADSCharacter::ClientRPC_RemoveItemData_Implementation(int32 IndexToRemove)
{
	//UDSGiftBox 를 가지고온다.
	if (HeldItem.IsValid())
	{
		if (IsLocallyControlled())
		{
			// 클라에서 UI업데이트
			HeldItem->RemoveItemUI(IndexToRemove);
		}
	}
}
void ADSCharacter::TryPickupItem(int32 ItemIdx)
{
	if (HeldItem.IsValid())
	{
		TArray<FDSItemInfo> ItemData = HeldItem->GetItemData();
		if (ItemData.IsValidIndex(ItemIdx))
		{
			FDSItemInfo ItemInfo = ItemData[ItemIdx];
			
			bool bIsItemStored = InventoryComponent->StoreItems(nullptr, ItemInfo);

			if (bIsItemStored)
			{
				ServerRPC_RemoveItemData(ItemIdx);
			}
		}
	}
}

void ADSCharacter::PickupItem(AActor* Interactor)
{
	ADSItemActor* SurroundingItem = Cast<ADSItemActor>(Interactor);

	if (IsValid(SurroundingItem))
	{
		TArray<FDSItemInfo> ItemData = SurroundingItem->GetItemData();

		bool bIsItemStored = InventoryComponent->StoreItems(SurroundingItem, ItemData[0]);
	}	
}

void ADSCharacter::SelectedItem(AActor* Interactor)
{
	
	ADSPlayerController* PlayerController = Cast<ADSPlayerController>(UDSGameUtils::GetPlayerController(this));
	
	if (IsValid(PlayerController))
	{
		PlayerController->SetUIFocusMode();
	}

	ADSGiftBox* SurroundingItem = Cast<ADSGiftBox>(Interactor);

	if (IsValid(SurroundingItem))
	{
		/*서버랑 클라이언트를 설정해두어야지, UI를 사용해서 인터랙션을 할 때 RPC 전송이 가능하다.*/
		/*SelectedItem은 클라이언트만 작동한다. => 그래서 서버에서 작동할 수 있도록 서버도 아이템으로 덮는다.*/
		//주변 아이템을 설정해준다, 클라이언트 설정
		HeldItem = SurroundingItem;
		
		ServerRPC_SetSurroundingItem(SurroundingItem);

		TArray<FDSItemInfo> ItemInfo = SurroundingItem->GetItemData();

		if (IsLocallyControlled())
		{
			SurroundingItem->PushItemListWidget(this);
		}

		DSEVENT_DELEGATE_INVOKE(SurroundingItem->OnUpdateItemWidget, ItemInfo);
	}
}

void ADSCharacter::SendItem(int32 PlayerId, int32 ItemID)
{
	ServerRPC_SendItme(PlayerId, ItemID);
}

void ADSCharacter::ServerRPC_SetSurroundingItem_Implementation(ADSGiftBox* Interactor)
{
	if (IsValid(Interactor))
	{
		//SurroundingItem이 실제 있다면,
		HeldItem = Interactor;
	}
}

void ADSCharacter::ServerRPC_SendItme_Implementation(int32 PlayerId, int32 ItemID)
{
	UWorld* World = GetWorld();
	check(World);

	if (PlayerId >= 0)
	{
		for (FConstPlayerControllerIterator Iterator = World->GetPlayerControllerIterator(); Iterator; ++Iterator)
		{
			APlayerController* PlayerController = Iterator->Get();

			DS_NETLOG(DSNetLog, Log, TEXT("PlayerController : %s"), *PlayerController->GetName());

			if (IsValid(PlayerController))
			{
				APlayerState* CurrentPlayerState = PlayerController->GetPlayerState<APlayerState>();
				ADSCharacter* Character = PlayerController->GetPawn<ADSCharacter>();
			
				if (IsValid(Character) && IsValid(CurrentPlayerState) && CurrentPlayerState->GetPlayerId() == PlayerId)
				{
					bool bIsItemStored = Character->StoreItem(ItemID);
					if (bIsItemStored)
					{
						DS_LOG(DSItemLog, Warning, TEXT("Store Item"));
						return;

					}
					else
					{
						DSEVENT_DELEGATE_INVOKE(InventoryComponent->OnInventoryFull, EAlertStatus::DeliveryFailed);
						InventoryComponent->RestoreItem(ItemID);
						if (false == HasAuthority())
						{
							ClientRPC_ItemSendFaild();
						}
						return;

					}
				}
			}
		}
	}
	
}

void ADSCharacter::ClientRPC_ItemSendFaild_Implementation()
{
	DSEVENT_DELEGATE_INVOKE(InventoryComponent->OnInventoryFull, EAlertStatus::DeliveryFailed);
}

void ADSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (IsLocallyControlled())
	{
		if (IsValid(PlayerInputComponent))
		{
			DSEVENT_DELEGATE_BIND(OnMovementModeChanged, DSPlayerInputComponent, &UDSPlayerInputComponent::OnInputMappingChanged);
			DSPlayerInputComponent->SetupInputComponent(InputComponent);
		}
		
		UDSCharacterMovementComponent* Movement = GetCharacterMovement<UDSCharacterMovementComponent>();

		if (IsValid(Movement))
		{
			DSEVENT_DELEGATE_BIND(OnMovementModeChanged, Movement, &UDSCharacterMovementComponent::HandleMovementModeChanged);
		}
	}

}

void ADSCharacter::BeginPlay()
{
	Super::BeginPlay();

	ADSPlayerController* LocalPlayerController = GetController<ADSPlayerController>();

	if (IsValid(LocalPlayerController))
	{
		EnableInput(LocalPlayerController);
	}

	if (false == IsValid(DodgeCurve))
	{
		DodgeCurve = DodgeCurveClass.LoadSynchronous();
	}

	if (IsValid(DodgeTimelineComponent))
	{
		//로드 되어있다면,
		FOnTimelineFloat DodgeTimelineFloat;
		
		DodgeTimelineFloat.BindUFunction(this, FName("DodgeInterp"));
		DodgeTimelineComponent->SetLooping(false);
		DodgeTimelineComponent->SetPlayRate(1.f);
		DodgeTimelineComponent->AddInterpFloat(DodgeCurve, DodgeTimelineFloat);
		
		FOnTimelineEvent OnDodgeFinish;
		OnDodgeFinish.BindUFunction(this, FName("OnDodgeFinished"));
		DodgeTimelineComponent->SetTimelineFinishedFunc(OnDodgeFinish);
	}

	// 카메라는 로컬일 때만 존재하기 때문에 설정
	if (GetLocalRole() == ENetRole::ROLE_Authority || IsLocallyControlled())
	{
		if (IsValid(CameraControlComponent))
		{
			CameraControlComponent->Initialize(CameraSpringArm, Camera);

			DSEVENT_DELEGATE_BIND(OnCameraModeChanged, CameraControlComponent, &UDSCameraControlComponent::UpdateCameraModePriority);
			DSEVENT_DELEGATE_BIND(OnZoomInStarted, CameraControlComponent, &UDSCameraControlComponent::ZoomIn);
			DSEVENT_DELEGATE_BIND(OnZoomOutStarted, CameraControlComponent, &UDSCameraControlComponent::ZoomOut);
		}	
	}
}


void ADSCharacter::OnDetectComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//카메라는 로컬에서만 조절되어져야 하기 때문에 로컬만 판단한다.
	if (false == IsLocallyControlled())
	{
		return;
	}
	//AI라면
	//인원수를 체크한다.
	//3명 이상이 되어지면, 전투모드로 판단해서 카메라 모드를 전투 모드로 변경한다.
	ADSCharacter* Enemy = Cast<ADSCharacter>(OtherActor);

	if (false == IsValid(Enemy))
	{
		return;
	}

	if (DetectedEnemies.Contains(Enemy))
	{
		return;
	}

	DetectedEnemies.Add(Enemy);

	if (DetectedEnemies.Num() >= 3)
	{
		//N명 이상일 때 전투모드로 변화한다.
		DSEVENT_DELEGATE_INVOKE(OnCameraModeChanged, ECameraMode::Combat, true);
	}

}

void ADSCharacter::OnDetectComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

	//AI라면
	//인원수를 체크한다.
	//3명 미만이라면, 탐색 모드로 판단해서 카메라 모드를 탐색 모드로 변경한다.
	
	//카메라는 로컬에서만 조절되어져야 하기 때문에 로컬만 판단한다.
	if (false == IsLocallyControlled())
	{
		return;
	}
	//AI라면
	//인원수를 체크한다.
	//3명 이상이 되어지면, 전투모드로 판단해서 카메라 모드를 전투 모드로 변경한다.
	ADSCharacter* Enemy = Cast<ADSCharacter>(OtherActor);

	if (false == IsValid(Enemy))
	{
		return;
	}

	if (DetectedEnemies.Contains(Enemy))
	{
		//적이 있다면 삭제
		DetectedEnemies.Remove(Enemy);

		if (DetectedEnemies.Num() < 2)
		{
			//적이 적어지면, 탐색모드로 변환
			DSEVENT_DELEGATE_INVOKE(OnCameraModeChanged, ECameraMode::Combat, false);
		}
	}
}

void ADSCharacter::SetJumpHeight(bool bIsRun)
{
	if (false == HasAuthority())
	{
		ServerRPC_SetJumpVelocity(bIsRun);
	}

	const float Gravity = 980.0f;
	float JumpVelocity = 0.f;

	if (bIsRun)
	{
		JumpVelocity = FMath::Sqrt(2 * Gravity * RunJumpHeight);
	}
	else
	{
		JumpVelocity = FMath::Sqrt(2 * Gravity * NomalJumpHeight);
	}
	GetCharacterMovement()->JumpZVelocity = JumpVelocity;

}

void ADSCharacter::ServerRPC_SetJumpVelocity_Implementation(bool bIsRun)
{
	SetJumpHeight(bIsRun);
}
