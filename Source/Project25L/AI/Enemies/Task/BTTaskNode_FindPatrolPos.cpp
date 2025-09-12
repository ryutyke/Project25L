// Default
#include "AI/Enemies/Task/BTTaskNode_FindPatrolPos.h"

// UE
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"

// Game

UBTTaskNode_FindPatrolPos::UBTTaskNode_FindPatrolPos()
	: Super()
	, Radius(1000.f)
	, PrePosition()
{
}

EBTNodeResult::Type UBTTaskNode_FindPatrolPos::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Type = Super::ExecuteTask(OwnerComp, NodeMemory);

	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();

	if (nullptr == ControllingPawn)
	{
		return EBTNodeResult::Failed;
	}

	PrePosition = ControllingPawn->GetActorLocation();

	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(ControllingPawn->GetWorld()); //컨트롤하는 Pawn이 위치한 월드를 가져옴

	if (nullptr == NavSystem)
	{
		return EBTNodeResult::Failed;
	}

	AAIController* AIController = OwnerComp.GetAIOwner();

	if (nullptr == AIController)
	{
		return EBTNodeResult::Failed;
	}

	FNavLocation NextPatrolPos;

	if (NavSystem->GetRandomPointInNavigableRadius(PrePosition, Radius, NextPatrolPos))
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsVector(TEXT("PatrolPos"), NextPatrolPos.Location);
		return Type;
	}

	return EBTNodeResult::Failed;
}
