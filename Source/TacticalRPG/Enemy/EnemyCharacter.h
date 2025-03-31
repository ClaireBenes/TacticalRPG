#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyCharacter.generated.h"

class AEnemyAIController;
class AGridManager;

UCLASS()
class TACTICALRPG_API AEnemyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemyCharacter();


protected:
    virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void TakeTurn();

	UFUNCTION()
	void SetPath(TArray<FVector2D> NewPath);

	bool GetIsMoving();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    AEnemyAIController* AIControllerRef = nullptr;

private:
	UPROPERTY(EditAnywhere, Category = "Movement")
	float MoveSpeed = 400.0f; // Units per second

	AGridManager* GridManager = nullptr;
	TArray<FVector2D> Path;
	bool bIsMoving = false;
};
