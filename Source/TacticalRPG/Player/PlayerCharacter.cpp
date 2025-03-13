#include "TacticalRPG/Player/PlayerCharacter.h"

#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

#include "TacticalRPG/Grid/GridManager.h"

#include "EngineUtils.h"

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
                FVector TargetLocation = FVector(ClickedCellX * GridSize, ClickedCellY * GridSize, GetActorLocation().Z);
                MoveToGridCell(TargetLocation);
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("Invalid Move: This tile is out of range!"));
            }
        }
    }
}

void APlayerCharacter::MoveToGridCell(FVector TargetLocation)
{
    SetActorLocation(TargetLocation);
}



