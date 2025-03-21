#include "TacticalRPG/Enemy/BehaviorTree/BTTask_FindTarget.h"

#include "TacticalRPG/Enemy/EnemyAIController.h"
#include "TacticalRPG/Player/PlayerCharacter.h"

#include "GameFramework/Actor.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

EBTNodeResult::Type UBTTask_FindTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AEnemyAIController* AIController = Cast<AEnemyAIController>(OwnerComp.GetAIOwner());
    if (!AIController) return EBTNodeResult::Failed;

    TArray<AActor*> PlayerCharacters;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerCharacter::StaticClass(), PlayerCharacters);

    AActor* ClosestTarget = nullptr;
    float MinDist = FLT_MAX;
    FVector EnemyPos = AIController->GetPawn()->GetActorLocation();

    for (AActor* Player : PlayerCharacters)
    {
        float Dist = FVector::Dist2D(EnemyPos, Player->GetActorLocation());
        if (Dist < MinDist)
        {
            MinDist = Dist;
            ClosestTarget = Player;
        }
    }

    if (ClosestTarget)
    {
        AIController->GetBlackboardComponent()->SetValueAsObject("TargetPlayer", ClosestTarget);
        return EBTNodeResult::Succeeded;
    }

    return EBTNodeResult::Failed;
}
