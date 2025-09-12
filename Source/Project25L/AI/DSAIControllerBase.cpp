// Default
#include "AI/DSAIControllerBase.h"

// UE
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BlackboardData.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISenseConfig_Sight.h"

// Game
#include "System/DSGameDataSubsystem.h"
#include "DSLogChannels.h"


ADSAIControllerBase::ADSAIControllerBase()
	: Super()
	, BehaviorTree(nullptr)
	, BlackboardData(nullptr)
{
	AIPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));

	UAISenseConfig_Sight* SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	AIPerception->ConfigureSense(*SightConfig);
}

void ADSAIControllerBase::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	LoadData();
}

void ADSAIControllerBase::LoadData()
{
	TArray<FSoftObjectPath> AssetsToLoad;
	AssetsToLoad.Add(BlackboardAsset.ToSoftObjectPath());
	AssetsToLoad.Add(BehaviorTreeAsset.ToSoftObjectPath());

	UDSGameDataSubsystem::StreamableManager.RequestAsyncLoad(AssetsToLoad, FStreamableDelegate::CreateLambda([WeakPtr = TWeakObjectPtr<ADSAIControllerBase>(this)]()
    {
        if (false == WeakPtr.IsValid())
        {
            return;
        }

        // 애셋 로드 후 포인터 설정
        WeakPtr->BlackboardData = WeakPtr->BlackboardAsset.Get();
        WeakPtr->BehaviorTree = WeakPtr->BehaviorTreeAsset.Get();


		//비동기 로딩이 끝났을 때 AI의 Blackboard, BehaviorTree를 설정할 수 있도록 한다.
		WeakPtr->RunAI();
    }));

}

void ADSAIControllerBase::RunAI()
{

	UBlackboardComponent* BCPtr = Blackboard.Get();

	//BlackboardData을 사용할 것으로 Blackboard 지정
	if (UseBlackboard(BlackboardData, BCPtr))
	{
		Blackboard->SetValueAsVector(TEXT("PatrolPos"), GetPawn()->GetActorLocation());
		//현재 Blackboard를 BehaviorTree가 실행하도록 함.
		bool RunResult = RunBehaviorTree(BehaviorTree);
		ensure(RunResult);
	}
	
}

void ADSAIControllerBase::StopAI()
{
	//BehaviorTree의 경우 BrainComponent에 저장
	UBehaviorTreeComponent* BehaviorTreeCPtr = Cast<UBehaviorTreeComponent>(BrainComponent);
	if (IsValid(BehaviorTreeCPtr))
	{
		BehaviorTreeCPtr->StopTree();
	}
}
