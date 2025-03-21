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
        TArray<FVector2D> Path = GridManager->FindPathToCell(
            GridManager->ConvertWorldToGrid(EnemyCharacter->GetActorLocation()),
            GridManager->ConvertWorldToGrid(Target->GetActorLocation())
        );

        //TODO : Manage to fix my pb and have Path.Num actually be superior at 0
        if (Path.Num() > 0)
        {
            EnemyCharacter->SetPath(Path);
            return EBTNodeResult::Succeeded;
        }
    }

    return EBTNodeResult::Failed;
}
