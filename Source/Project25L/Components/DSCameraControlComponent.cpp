// Default
#include "Components/DSCameraControlComponent.h"

// UE
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/TimelineComponent.h"

// Game
#include "Character/Characters/DSCharacter.h"
#include "System/DSEventSystems.h"
#include "DSLogChannels.h"

UDSCameraControlComponent::UDSCameraControlComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, bIsZoomed(false)
	, CameraDistance()
	, CameraRotation()
	, SocketOffset()
	, CameraCurve(nullptr)
	, CameraSpringArm(nullptr)
	, Camera(nullptr)
{
	CameraTimelineComponent = CreateDefaultSubobject<UTimelineComponent>(TEXT("CameraTimelineComponent"));

	CameraSettings.Add({ ECameraMode::Default, FDSCameraSetting(300.f, FRotator(-15.f, 0.f, 0.f), FVector(0.f,0.f,100.f)) });
	CameraSettings.Add({ ECameraMode::UpHill, FDSCameraSetting(300.f, FRotator(15.f, 0.f, 0.f), FVector(0.f,0.f,-30.f)) });
	CameraSettings.Add({ ECameraMode::DownHill, FDSCameraSetting(300.f, FRotator(-30.f, 0.f, 0.f), FVector(0.f,0.f,280.f)) });
	CameraSettings.Add({ ECameraMode::Combat, FDSCameraSetting(500.f,FRotator(-15.f, 0.f, 0.f), FVector(0.f,0.f,100.f)) });
	CameraSettings.Add({ ECameraMode::Zoom, FDSCameraSetting(50.f, FRotator(-30.f, 0.f, 0.f), FVector(0.f, 40.f, 80.f)) });
}

void UDSCameraControlComponent::Initialize(USpringArmComponent* InSpringArm, UCameraComponent* InCamera)
{
	CameraSpringArm = InSpringArm;
	Camera = InCamera;

	//카메라는 로컬에만 조절할 수 있다.
	if (false == IsValid(CameraCurve))
	{
		CameraCurve = CameraCurveClass.LoadSynchronous();
	}

	if (IsValid(CameraTimelineComponent))
	{
		//로드 되어있다면,
		FOnTimelineFloat CameraTimelineFloat;

		CameraTimelineFloat.BindUFunction(this, FName("InterpolateCameraMovement"));
		CameraTimelineComponent->SetLooping(false);
		CameraTimelineComponent->SetPlayRate(5.f);
		CameraTimelineComponent->AddInterpFloat(CameraCurve, CameraTimelineFloat);
	}
}

void UDSCameraControlComponent::OnZoomByMouseWheel(float InputValue)
{
	if (false == IsValid(CameraSpringArm))
	{
		return;
	}

	if (bIsZoomed)
	{
		UWorld* World = GetWorld();
		check(World);

		float NewLength = CameraSpringArm->TargetArmLength + InputValue * 10.f;

		// 최소/최대 거리 제한
		NewLength = FMath::Clamp(NewLength, 10.f, 50.f);
		CameraSpringArm->TargetArmLength = NewLength;
	}
}

void UDSCameraControlComponent::ApplyCameraModeByIncline(const ETerrainInclineType& InclineType, bool bIsPending)
{
	ECameraMode CameraMode = ECameraMode::Default;

	switch (InclineType)
	{
	case ETerrainInclineType::DownHill:
		CameraMode = ECameraMode::DownHill;
		break;
	case ETerrainInclineType::UpHill:
		CameraMode = ECameraMode::UpHill;
		break;
	}

	UpdateCameraModePriority(CameraMode, bIsPending);
}

void UDSCameraControlComponent::ZoomIn()
{
	if (bIsZoomed)
	{
		//Zoom이 켜져있는데 ZoomIn을 호출할 경우
		return;
	}
	ADSCharacter* Character = Cast<ADSCharacter>(GetOwner());
	
	if (false == IsValid(Character))
	{
		return;
	}

	bIsZoomed = true;

	UpdateCameraModePriority(ECameraMode::Zoom, bIsZoomed);

	Character->ServerRPC_EnableControllerRotationYaw(true);
}

void UDSCameraControlComponent::ZoomOut()
{
	ADSCharacter* Character = Cast<ADSCharacter>(GetOwner());

	if (false == IsValid(Character))
	{
		return;
	}

	bIsZoomed = false;

	//Zoom 제거 - 제거용도
	UpdateCameraModePriority(ECameraMode::Zoom, bIsZoomed);

	Character->ServerRPC_EnableControllerRotationYaw(false);
}

void UDSCameraControlComponent::UpdateCameraModePriority(const ECameraMode& CamereMode, bool bIsPending)
{
	if (bIsPending)
	{
		//우선순위에 넣어둔다. => enum값을 기준으로 우선순위가 결정되어진다.
		PriorityQueue.HeapPush(CamereMode);
	}
	else
	{
		//지정된 카메라 모드를 제거한다.
		PriorityQueue.Remove(CamereMode);
	}

	UpdateCameraDistanceByMode();
}

void UDSCameraControlComponent::UpdateCameraDistanceByMode()
{
	ECameraMode CameraMode = ECameraMode::Default;

	//가장 우선순위가 높은 카메라 모드로 교체해준다.
	if (false == PriorityQueue.IsEmpty())
	{
		CameraMode = PriorityQueue.HeapTop();
	}

	if (false == CameraSettings.Contains(CameraMode))
	{
		return;
	}

	if (false == IsValid(CameraSpringArm))
	{
		return;
	}

	const FDSCameraSetting& CameraSetting = CameraSettings[CameraMode];
	CameraDistance.Key = CameraSetting.TargetDistance;
	CameraDistance.Value = CameraSpringArm->TargetArmLength;

	SocketOffset.Key = CameraSetting.TargetSocketOffset;
	SocketOffset.Value = CameraSpringArm->SocketOffset;
	
	CameraRotation.Key = CameraSetting.TargetRotation;
	CameraRotation.Value = Camera->GetRelativeRotation();

	if (IsValid(CameraTimelineComponent))
	{
		CameraTimelineComponent->PlayFromStart();
	}
}

void UDSCameraControlComponent::InterpolateCameraMovement(float CurveValue)
{
	if (IsValid(CameraSpringArm))
	{
		const float& NewLength = FMath::Lerp(CameraDistance.Key, CameraDistance.Value, CurveValue);
		// 새로운 길이 조절
		CameraSpringArm->TargetArmLength = NewLength;

		const FVector& NewSocketOffset = FMath::Lerp(SocketOffset.Key, SocketOffset.Value, CurveValue);
		CameraSpringArm->SocketOffset = NewSocketOffset;
	}

	if (IsValid(Camera))
	{
		// 구형 lerp로 회전 조절
		const FRotator& NewRotator = FQuat::Slerp(CameraRotation.Key.Quaternion(), CameraRotation.Value.Quaternion(), CurveValue).Rotator();
		Camera->SetRelativeRotation(NewRotator);
	}
}