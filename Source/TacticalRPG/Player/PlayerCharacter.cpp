#include "TacticalRPG/Player/PlayerCharacter.h"

#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

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
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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
    if (!IsValid(PlayerController)) return;

    // Get mouse position in the world
    FVector WorldLocation, WorldDirection;
    if (PlayerController->DeprojectMousePositionToWorld(WorldLocation, WorldDirection))
    {
        // Perform a line trace to detect grid cells
        FVector Start = WorldLocation;
        FVector End = Start + (WorldDirection * 10000); // Cast ray far enough to hit something

        FHitResult HitResult;
        FCollisionQueryParams Params;
        Params.AddIgnoredActor(this); // Ignore the character itself

        if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params))
        {
            FVector TargetLocation = HitResult.Location;

            // Snap to nearest grid cell
            int ClickedCellX = FMath::RoundToInt(TargetLocation.X / GridSize) * GridSize;
            int ClickedCellY = FMath::RoundToInt(TargetLocation.Y / GridSize) * GridSize;
            TargetLocation.Z = GetActorLocation().Z; // Keep same height

            FVector2D ClickedCell(ClickedCellX, ClickedCellY);

            // Check if movement is allowed
            if (IsCellWithinRange(TargetLocation))
            {
                MoveToGridCell(TargetLocation);
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("Invalid Move: Out of range!"));
            }
        }
    }
}

void APlayerCharacter::MoveToGridCell(FVector TargetLocation)
{
    SetActorLocation(TargetLocation);
}

bool APlayerCharacter::IsCellWithinRange(FVector TargetLocation)
{
    // Get the character's current position in grid coordinates
    FVector CurrentLocation = GetActorLocation();
    int PlayerX = FMath::RoundToInt(CurrentLocation.X / GridSize);
    int PlayerY = FMath::RoundToInt(CurrentLocation.Y / GridSize);

    // Calculate Manhattan distance
    int DistanceX = FMath::Abs(PlayerX - CellIndex.X);
    int DistanceY = FMath::Abs(PlayerY - CellIndex.Y);

    // Check if the cell is within range
    bool bIsWithinRange = (DistanceX + DistanceY) <= MaxMoveRange;

    // Now, check if the tile is actually green (valid move)
    if (!bIsWithinRange)
    {
        return false;
    }

    // Here, we add an additional check: Is the tile REALLY a valid move tile?
    FVector TileWorldPosition = FVector(CellIndex.X * CellSize, CellIndex.Y * CellSize, GetActorLocation().Z);

    // Perform a trace to check if the tile is green (this assumes debug colors are temporary)
    FHitResult HitResult;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this); // Ignore self

    if (GetWorld()->LineTraceSingleByChannel(HitResult, TileWorldPosition + FVector(0, 0, 50), TileWorldPosition - FVector(0, 0, 50), ECC_Visibility, Params))
    {
        // If the hit object has a valid material/color for "green", allow movement
        if (HitResult.Actor.IsValid() && HitResult.Actor->ActorHasTag("ValidTile"))
        {
            return true;
        }
    }

    // Otherwise, block movement to the tile
    return false;
}



