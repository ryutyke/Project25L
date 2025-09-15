#pragma once

// Default
#include "CoreMinimal.h"

// UE
#include "BehaviorTree/BTDecorator.h"

// UHT
#include "BTDecorator_CanMove.generated.h"


UCLASS()
class PROJECT25L_API UBTDecorator_CanMove : public UBTDecorator
{
	GENERATED_BODY()
	
public:
	UBTDecorator_CanMove();

	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const;
};
