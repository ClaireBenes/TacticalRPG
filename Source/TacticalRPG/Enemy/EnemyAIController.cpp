#include "TacticalRPG/Enemy/EnemyAIController.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"

void AEnemyAIController::BeginPlay()
{
    Super::BeginPlay();

    if (IsValid(EnemyBT))
    {
        RunBehaviorTree(EnemyBT);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("BehaviorTree is NULL in AIController!"));
    }
}

void AEnemyAIController::StartEnemyTurn()
{
    GetBlackboardComponent()->SetValueAsBool("bIsTurn", true);
}
