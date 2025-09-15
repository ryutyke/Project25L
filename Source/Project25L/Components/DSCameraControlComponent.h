#pragma once

// Default
#include "CoreMinimal.h"

// UE
#include "Components/PawnComponent.h"
#include "GameData/DSEnums.h"

// UHT
#include "DSCameraControlComponent.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UTimelineComponent;
class USceneComponent;

USTRUCT(BlueprintType)
struct FDSCameraSetting
{
	GENERATED_USTRUCT_BODY()

public:
	FDSCameraSetting() 
		: TargetDistance(0.f)
		, TargetRotation(FRotator::ZeroRotator)
		, TargetSocketOffset(FVector::ZeroVector)
		{}

	FDSCameraSetting(const float& Distance,const FRotator& Rotator, const FVector& SocketOffset)
		: TargetDistance(Distance)
		, TargetRotation(Rotator)
		, TargetSocketOffset(SocketOffset)
	{}
	
	UPROPERTY(EditAnywhere, Category = "DSSettings | Camera")
	float TargetDistance;

	UPROPERTY(EditAnywhere, Category = "DSSettings | Camera")
	FRotator TargetRotation;

	UPROPERTY(EditAnywhere, Category = "DSSettings | Camera")
	FVector TargetSocketOffset;
};

UCLASS()
class PROJECT25L_API UDSCameraControlComponent : public UPawnComponent
{
	GENERATED_BODY()

public:
	UDSCameraControlComponent(const FObjectInitializer& ObjectInitializer);

	void Initialize(USpringArmComponent* InSpringArm, UCameraComponent* InCamera);
	void OnZoomByMouseWheel(float InputValue);
	void ApplyCameraModeByIncline(const ETerrainInclineType& InclineType, bool bIsPending);
public:
	void ZoomIn();
	void ZoomOut();
	// Params CameraMode : 실제 카메라의 모드
	// 삽입 여부, CameraMode 삽입/ CameraMode 제거 용도로 사용
	void UpdateCameraModePriority(const ECameraMode& CamereMode, bool bIsPending);
	
protected:
	// 가장 우선순위가 높은 카메라 모드를 적용
	void UpdateCameraDistanceByMode();

	UFUNCTION()
	void InterpolateCameraMovement(float CurveValue);

protected:

	UPROPERTY(Transient)
	TArray<ECameraMode> PriorityQueue;
	
protected:
	/*Camera System*/
	UPROPERTY(Transient)
	uint8 bIsZoomed : 1;

	TTuple<float, float> CameraDistance;

	TTuple<FRotator, FRotator> CameraRotation;

	TTuple<FVector, FVector> SocketOffset;

	// struct - FCameraSetting 과 유사 
	UPROPERTY(Transient)
	TObjectPtr<UCurveFloat> CameraCurve;

	UPROPERTY(EditAnywhere, Category ="DSSettings | Camera")
	TMap<ECameraMode, FDSCameraSetting> CameraSettings;

	UPROPERTY(EditAnywhere, Category = "DSSettings | Camera")
	TSoftObjectPtr<UCurveFloat> CameraCurveClass;

	UPROPERTY(EditAnywhere, Category = "DSSettings | Camera", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UTimelineComponent> CameraTimelineComponent;

protected:
	/*Spring Arm, Camera*/
	UPROPERTY(Transient)
	TObjectPtr<USpringArmComponent> CameraSpringArm;

	UPROPERTY(Transient)
	TObjectPtr<UCameraComponent> Camera;
};
