#pragma once

// Defualt
#include "CoreMinimal.h"

// UE
#include "BehaviorTree/BTService.h"

// UHT
#include "BTService_StopMove.generated.h"

UCLASS()
class PROJECT25L_API UBTService_StopMove : public UBTService
{
	GENERATED_BODY()
	
public:
	UBTService_StopMove();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

};
