// Default
#include "AI/Enemies/Decorator/BTDecorator_CanMove.h"

// UE
#include "BehaviorTree/BlackboardComponent.h"

UBTDecorator_CanMove::UBTDecorator_CanMove()
{
	NodeName = TEXT("Can Move");
}

bool UBTDecorator_CanMove::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	UBlackboardComponent* BC = OwnerComp.GetBlackboardComponent();

	bool bIsFalling = false;
	if (IsValid(BC))
	{
		bIsFalling = BC->GetValueAsBool(TEXT("bIsFalling"));
	}

	//falling이 true가 아니면, 함수를 실행한다.
	return bIsFalling != true;
}
