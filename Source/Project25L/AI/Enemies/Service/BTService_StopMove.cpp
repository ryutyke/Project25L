// Default
#include "AI/Enemies/Service/BTService_StopMove.h"

// UE
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

// Game

UBTService_StopMove::UBTService_StopMove()
{
	NodeName = TEXT("Stop Movement");
	Interval = 1.0f;
}

void UBTService_StopMove::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	AAIController* AIController = OwnerComp.GetAIOwner();

	if (false == IsValid(AIController))
	{
		return;
	}

	ACharacter* ControllingPawn = Cast<ACharacter>(AIController->GetPawn());

	if (false == IsValid(ControllingPawn))
	{
		return;
	}

	UCharacterMovementComponent* Movement = ControllingPawn->GetCharacterMovement();

	//스킬 적용 중
	if (IsValid(Movement))
	{
		bool bIsFalling = Movement->MovementMode == EMovementMode::MOVE_Flying;
		
		if (bIsFalling)
		{
			//움직이고 있다면, 움직임을 취소한다.
			Movement->StopMovementImmediately();
		}
		else
		{
			UBlackboardComponent* BC = OwnerComp.GetBlackboardComponent();

			if (IsValid(BC))
			{
				BC->SetValueAsBool(TEXT("bIsFalling"), bIsFalling);
			}
		}
	}

}
