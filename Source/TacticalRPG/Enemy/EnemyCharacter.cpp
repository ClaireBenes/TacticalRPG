#include "TacticalRPG/Enemy/EnemyCharacter.h"

#include "TacticalRPG/Enemy/EnemyAIController.h"
#include "TacticalRPG/Grid/GridManager.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "EngineUtils.h"

// Sets default values
AEnemyCharacter::AEnemyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	AIControllerRef = Cast<AEnemyAIController>(GetController());	

	if (!AIControllerRef)
	{
		UE_LOG(LogTemp, Error, TEXT("AIControllerRef is NULL in AEnemyCharacter!"));
	}

    // Find the GridManager in the world
    for (TActorIterator<AGridManager> It(GetWorld()); It; ++It)
    {
        GridManager = *It;
        break; // Stop after finding the first instance
    }
}

void AEnemyCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bIsMoving && Path.Num() > 0)
    {
        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Cyan, TEXT("Moving"));

        FVector CurrentLocation = GetActorLocation();

        FVector NextLocation = FVector(
            Path[0].X * GridManager->GridData->GridSizeX, 
            Path[0].Y * GridManager->GridData->GridSizeY, 
            GetActorLocation().Z);

        SetActorLocation(FMath::VInterpConstantTo(CurrentLocation, NextLocation, DeltaTime, MoveSpeed));

        FVector MoveDirection = (NextLocation - CurrentLocation).GetSafeNormal();
        if (!MoveDirection.IsNearlyZero())
        {
            FRotator TargetRotation = MoveDirection.Rotation();
            TargetRotation.Yaw -= 90.0f;
            FRotator NewRotation = FMath::RInterpTo(GetActorRotation(), TargetRotation, DeltaTime, 10.0f);
            SetActorRotation(NewRotation);
        }

        if (FVector::Dist(CurrentLocation, NextLocation) < 5.0f)
        {
            Path.RemoveAt(0);
            if (Path.Num() == 0)
            {
                SetActorLocation(NextLocation);
                bIsMoving = false;

                if (AIControllerRef && AIControllerRef->GetBlackboardComponent())
                {
                    AIControllerRef->GetBlackboardComponent()->SetValueAsBool("bIsMoving", false);
                }

                UE_LOG(LogTemp, Warning, TEXT("Enemy finished moving!"));
            }
        }
    }
}

void AEnemyCharacter::TakeTurn()
{
	if (IsValid(AIControllerRef))
	{
		AIControllerRef->StartEnemyTurn();
	}
}

void AEnemyCharacter::SetPath(TArray<FVector2D> NewPath)
{
	Path = NewPath;
	bIsMoving = true;

    // Update Blackboard
    AEnemyAIController* AIController = Cast<AEnemyAIController>(GetController());
    if (AIController && AIController->GetBlackboardComponent())
    {
        AIController->GetBlackboardComponent()->SetValueAsBool("bIsMoving", true);
    }
}

bool AEnemyCharacter::GetIsMoving()
{
    return bIsMoving;
}

