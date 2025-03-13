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

	PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		PlayerController->bShowMouseCursor = true; // Show cursor for click-based movement
	}

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

    if (bIsMoving)
    {
        FVector CurrentLocation = GetActorLocation();

        // Move smoothly using interpolation
        FVector NewLocation = FMath::VInterpConstantTo(CurrentLocation, TargetLocation, DeltaTime, MoveSpeed);
        SetActorLocation(NewLocation);

        // Check if the character has reached the destination
        if (FVector::Dist(CurrentLocation, TargetLocation) < 5.0f)
        {
            SetActorLocation(TargetLocation); // Snap to final position
            bIsMoving = false; // Stop moving

            // Show movement grid again
            GridManager->UpdateGridPosition();
        }
    }
}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Bind the left mouse click action
	PlayerInputComponent->BindAction("ClickMove", IE_Pressed, this, &APlayerCharacter::OnClickMove);
}

void APlayerCharacter::OnClickMove()
{
    if (!IsValid(PlayerController) || !IsValid(GridManager)) return;

    // Prevent movement if already moving
    if (bIsMoving)
    {
        UE_LOG(LogTemp, Warning, TEXT("Can't move: Character is still moving!"));
        return;
    }

    // Get mouse position in the world
    FVector WorldLocation, WorldDirection;
    if (PlayerController->DeprojectMousePositionToWorld(WorldLocation, WorldDirection))
    {
        FVector Start = WorldLocation;
        FVector End = Start + (WorldDirection * 10000);

        FHitResult HitResult;
        if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility))
        {
            FVector ClickedLocation = HitResult.Location;

            int ClickedCellX = FMath::RoundToInt(ClickedLocation.X / GridSize);
            int ClickedCellY = FMath::RoundToInt(ClickedLocation.Y / GridSize);
            FVector2D ClickedCell(ClickedCellX, ClickedCellY);

            // Ask GridManager if this move is valid
            if (GridManager->IsCellInRange(ClickedCell))
            {
                FVector NextTargetLocation = FVector(ClickedCellX * GridSize, ClickedCellY * GridSize, GetActorLocation().Z);

                // Hide movement grid before starting movement
                GridManager->HideMovementGrid();

                MoveToGridCell(NextTargetLocation);
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("Invalid Move: This tile is out of range!"));
            }
        }
    }
}

void APlayerCharacter::MoveToGridCell(FVector NewTargetLocation)
{
    TargetLocation = NewTargetLocation; // Store the new movement target
    bIsMoving = true; // Enable movement logic
}



