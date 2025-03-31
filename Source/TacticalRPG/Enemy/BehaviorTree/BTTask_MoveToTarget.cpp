#include "TacticalRPG/Enemy/BehaviorTree/BTTask_MoveToTarget.h"

#include "TacticalRPG/Enemy/EnemyAIController.h"
#include "TacticalRPG/Enemy/EnemyCharacter.h"
#include "TacticalRPG/Grid/GridManager.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

EBTNodeResult::Type UBTTask_MoveToTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AGridManager* GridManager = Cast<AGridManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AGridManager::StaticClass()));
    if (!GridManager) return EBTNodeResult::Failed;

    AEnemyAIController* AIController = Cast<AEnemyAIController>(OwnerComp.GetAIOwner());
    if (!AIController) return EBTNodeResult::Failed;

    AActor* Target = Cast<AActor>(AIController->GetBlackboardComponent()->GetValueAsObject("TargetPlayer"));
    if (!Target) return EBTNodeResult::Failed;

    AEnemyCharacter* EnemyCharacter = Cast<AEnemyCharacter>(AIController->GetPawn());

    if (EnemyCharacter && GridManager)
    {
        FVector2D EnemyGridPos = GridManager->ConvertWorldToGrid(EnemyCharacter->GetActorLocation());
        FVector2D TargetGridPos = GridManager->ConvertWorldToGrid(Target->GetActorLocation()); 

        TArray<FVector2D> Path = GridManager->FindPathToCell(EnemyGridPos, TargetGridPos);

        UE_LOG(LogTemp, Warning, TEXT("Generated Path Size: %d"), Path.Num()); // Debugging


        //TODO : Manage to fix my pb and have Path.Num actually be superior at 0
        if (Path.Num() > 0)
        {
            EnemyCharacter->SetPath(Path);
            // Set a flag in blackboard so the BT knows AI is moving
            //AIController->GetBlackboardComponent()->SetValueAsBool("bIsMoving", true);

            // Continue execution while AI moves
            return EBTNodeResult::InProgress;
            //return EBTNodeResult::Succeeded;
        }
    }

    return EBTNodeResult::Failed;
}

void UBTTask_MoveToTarget::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    AEnemyCharacter* EnemyCharacter = Cast<AEnemyCharacter>(OwnerComp.GetAIOwner()->GetPawn());
    if (!EnemyCharacter) return;

    // Check if enemy finished moving
    if (!EnemyCharacter->GetIsMoving())
    {
        UE_LOG(LogTemp, Warning, TEXT("Enemy reached target, finishing task."));
        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
    }
}
