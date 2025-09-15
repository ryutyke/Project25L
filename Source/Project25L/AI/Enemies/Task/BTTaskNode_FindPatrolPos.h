#pragma once

// Default
#include "CoreMinimal.h"

// UE
#include "BehaviorTree/BTTaskNode.h"

// UHT
#include "BTTaskNode_FindPatrolPos.generated.h"

UCLASS()
class PROJECT25L_API UBTTaskNode_FindPatrolPos : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTaskNode_FindPatrolPos();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY(EditAnywhere, Category = "DSSettings | PatrolRadius")
	float Radius;

	UPROPERTY(Transient)
	FVector PrePosition;
};
