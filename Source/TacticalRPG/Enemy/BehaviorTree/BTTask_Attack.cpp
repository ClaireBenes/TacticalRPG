#include "TacticalRPG/Enemy/BehaviorTree/BTTask_Attack.h"

EBTNodeResult::Type UBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UE_LOG(LogTemp, Warning, TEXT("Enemy Attacks!"));
	return EBTNodeResult::Succeeded;
}
