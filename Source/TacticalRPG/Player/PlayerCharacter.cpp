#include "TacticalRPG/Player/PlayerCharacter.h"

#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

#include "TacticalRPG/Grid/GridManager.h"

#include "EngineUtils.h"

// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create the SpringArm
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->bDoCollisionTest = false; // Prevent the camera from clipping through walls

	// Create the Camera
	TopDownCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCamera->SetupAttachment(SpringArm);
	TopDownCamera->bUsePawnControlRotation = false; // Prevent the camera from rotating with the character

}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

    // Find the GridManager in the world
    for (TActorIterator<AGridManager> It(GetWorld()); It; ++It)
    {
        GridManager = *It;
        break; // Stop after finding the first instance
    }
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    if (bIsMoving && Path.Num() > 0)
    {
        FVector CurrentLocation = GetActorLocation();

        // Move smoothly using interpolation
        FVector NextLocation = FVector(Path[0].X * GridManager->GetGridSize(), Path[0].Y * GridManager->GetGridSize(), GetActorLocation().Z);

        SetActorLocation(FMath::VInterpConstantTo(CurrentLocation, NextLocation, DeltaTime, MoveSpeed));

        // Rotate Character to Face the Target
        FVector MoveDirection = (NextLocation - CurrentLocation).GetSafeNormal();
        if (!MoveDirection.IsNearlyZero()) // Ensure we have a valid direction
        {
            FRotator TargetRotation = MoveDirection.Rotation();

            // Apply Offset
            TargetRotation.Yaw -= 90.0f;

            FRotator NewRotation = FMath::RInterpTo(GetMesh()->GetComponentRotation(), TargetRotation, DeltaTime, 10.0f); // Smooth rotation speed
            GetMesh()->SetWorldRotation(NewRotation); // Rotate only the mesh
        }

        // Check if the character has reached the destination
        if (FVector::Dist(CurrentLocation, NextLocation) < 5.0f)
        {
            Path.RemoveAt(0);
            if (Path.Num() == 0)
            {
                SetActorLocation(NextLocation); // Snap to final position
                bIsMoving = false; // Stop moving

                // Show movement grid again
                GridManager->UpdateGridPosition();
                Idle();
            }        
        }
    }
}

void APlayerCharacter::MoveToGridCell(FVector NewTargetLocation)
{
    TargetLocation = NewTargetLocation; // Store the new movement target
}

void APlayerCharacter::SetPath(TArray<FVector2D> NewPath)
{
    Path = NewPath;
    bIsMoving = true;

    Walk();
}



