#pragma once

// Default
#include "CoreMinimal.h"

// UE
#include "AIController.h"

// UHT
#include "DSAIControllerBase.generated.h"

class UAIPerceptionComponent;
class UBehaviorTree;
class UBlackboardData;

UCLASS()
class PROJECT25L_API ADSAIControllerBase : public AAIController
{
	GENERATED_BODY()
	
public:
	ADSAIControllerBase();

	virtual void OnPossess(APawn* InPawn) override;

	//비동기 데이터 로드, BehaviorTree, Blackboard Data
	virtual void LoadData();

protected:

	void RunAI();
	void StopAI();

protected:
	UPROPERTY(Transient)
	TObjectPtr<UBehaviorTree> BehaviorTree;

	UPROPERTY(Transient)
	TObjectPtr<UBlackboardData> BlackboardData;

	UPROPERTY(EditAnywhere, Category = "DSSettings | AIAsset")
	TSoftObjectPtr<UBehaviorTree> BehaviorTreeAsset;

	UPROPERTY(EditAnywhere, Category = "DSSettings | AIAsset")
	TSoftObjectPtr<UBlackboardData> BlackboardAsset;

	UPROPERTY(VisibleAnywhere, Category = "DSSettings | AI")
	TObjectPtr<UAIPerceptionComponent> AIPerception;

	
};
