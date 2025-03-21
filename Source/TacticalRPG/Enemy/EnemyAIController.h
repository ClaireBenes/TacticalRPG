#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyAIController.generated.h"

/**
 * 
 */
UCLASS()
class TACTICALRPG_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;

public:
	void StartEnemyTurn();

	UPROPERTY(EditAnywhere, Category = "Behavior Tree")
	UBehaviorTree* EnemyBT = nullptr;
};
