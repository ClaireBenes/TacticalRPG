#include "TacticalRPG/Grid/GridManager.h"

#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

#include "TacticalRPG/Player/PlayerCharacter.h"
#include "TacticalRPG/Player/PlayerCharacterController.h"

// Sets default values
AGridManager::AGridManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AGridManager::BeginPlay()
{
	Super::BeginPlay();
	
	PlayerController = GetWorld()->GetFirstPlayerController();
    //PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	GenerateGrid();
}

// Called every frame
void AGridManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
    UpdateGridPosition();
	UpdateHoveredCell();
}

void AGridManager::GenerateGrid()
{
	for (int X = 0; X < GridSizeX; X++)
	{
		for (int Y = 0; Y < GridSizeY; Y++)
		{
			FVector CellLocation = FVector(X * CellSize, Y * CellSize, 0);
		}
	}
}

void AGridManager::UpdateGridPosition()
{
    if (!IsValid(ControlledCharacter) || ControlledCharacter->IsCharacterMoving())
    {
        return; // Do NOT update grid if character is moving
    }

    FVector PlayerLocation = ControlledCharacter->GetActorLocation();
    PlayerLocation.X = FMath::RoundToInt(PlayerLocation.X / CellSize) * CellSize;
    PlayerLocation.Y = FMath::RoundToInt(PlayerLocation.Y / CellSize) * CellSize;
    PlayerLocation.Z = 0;

    // Clear previous debug grid
    FlushPersistentDebugLines(GetWorld());
    ValidCells.Empty();

    // Draw the grid centered around the player
    for (int X = -GridSizeX / 2; X <= GridSizeX / 2; X++)
    {
        for (int Y = -GridSizeY / 2; Y <= GridSizeY / 2; Y++)
        {
            FVector CellLocation = PlayerLocation + FVector(X * CellSize, Y * CellSize, 0);
            FVector2D CellIndex = FVector2D(X + (PlayerLocation.X / CellSize), Y + (PlayerLocation.Y / CellSize));

            // Create an invisible actor for tiles that can be moved onto
            if (IsCellInRange(CellIndex))
            {
                ValidCells.Add(CellIndex); // Store only valid movement tiles
            }
        }
    }

    // Identify Edge Cells
    for (FVector2D Cell : ValidCells)
    {
        FVector CellWorldLocation = FVector(Cell.X * CellSize, Cell.Y * CellSize, 5);
        FVector TopLeft = CellWorldLocation + FVector(-CellSize / 2, CellSize / 2, 0);
        FVector TopRight = CellWorldLocation + FVector(CellSize / 2, CellSize / 2, 0);
        FVector BottomLeft = CellWorldLocation + FVector(-CellSize / 2, -CellSize / 2, 0);
        FVector BottomRight = CellWorldLocation + FVector(CellSize / 2, -CellSize / 2, 0);

        // Check adjacent tiles
        FVector2D Neighbors[4] = {
            FVector2D(Cell.X + 1, Cell.Y),
            FVector2D(Cell.X - 1, Cell.Y),
            FVector2D(Cell.X, Cell.Y + 1),
            FVector2D(Cell.X, Cell.Y - 1)
        };

        if (!ValidCells.Contains(Neighbors[0])) // Right Edge
        {
            DrawDebugLine(GetWorld(), TopRight, BottomRight, FColor::Green, true, -1, 0, 3);
        }
        if (!ValidCells.Contains(Neighbors[1])) // Left Edge
        {
            DrawDebugLine(GetWorld(), TopLeft, BottomLeft, FColor::Green, true, -1, 0, 3);
        }
        if (!ValidCells.Contains(Neighbors[2])) // Top Edge
        {
            DrawDebugLine(GetWorld(), TopLeft, TopRight, FColor::Green, true, -1, 0, 3);
        }
        if (!ValidCells.Contains(Neighbors[3])) // Bottom Edge
        {
            DrawDebugLine(GetWorld(), BottomLeft, BottomRight, FColor::Green, true, -1, 0, 3);
        }
    }
}

void AGridManager::UpdateHoveredCell()
{
    if (!IsValid(PlayerController)) return;

    if (IsValid(ControlledCharacter) && ControlledCharacter->IsCharacterMoving())
    {
        return; // Do NOT update hovered cell if character is moving
    }

    FVector WorldLocation, WorldDirection;
    if (PlayerController->DeprojectMousePositionToWorld(WorldLocation, WorldDirection))
    {
        FVector Start = WorldLocation;
        FVector End = Start + (WorldDirection * 10000);

        FHitResult HitResult;
        if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility))
        {
            FVector HitLocation = HitResult.Location;

            AActor* HitActor = HitResult.GetActor();

            int CellX = FMath::RoundToInt(HitLocation.X / CellSize);
            int CellY = FMath::RoundToInt(HitLocation.Y / CellSize);

            FVector2D CellIndex(CellX, CellY);

            bool bCanClick = false;

            APlayerCharacterController* PlayerCharacterController = Cast<APlayerCharacterController>(PlayerController);

            // Check if cell is in move range OR Touching a character
            if((HitActor != nullptr && Cast<APlayerCharacter>(HitActor)) || IsCellInRange(CellIndex))
            {
                bCanClick = true;
          
                if(IsValid(PlayerCharacterController))
                {
                    PlayerCharacterController->HoveredCharacter = Cast<APlayerCharacter>(HitActor);
                }
            }
            else
            {
                PlayerCharacterController->HoveredCharacter = nullptr;
            }

            // Draw the hover outline (Green = Valid, Red = Invalid)
            DrawDebugBox(GetWorld(), FVector(CellX * CellSize, CellY * CellSize, 5), FVector(CellSize / 2, CellSize / 2, 5), bCanClick ? FColor::Green : FColor::Red, false, -1, 0, 5);
            
            DrawDebugLine(GetWorld(), Start, HitLocation, FColor::Green, false, -1.0f);
            DrawDebugPoint(GetWorld(), HitLocation, 5.0f, FColor::Green, false, -1.0f);
        }
        else
        {
            DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, -1.0f);
        }
    }
}

void AGridManager::HideMovementGrid()
{
    // Completely override all drawn debug lines by redrawing with transparent color
    FlushPersistentDebugLines(GetWorld());
    ValidCells.Empty();
}

bool AGridManager::IsCellInRange(FVector2D CellIndex)
{
    // Get character position
    if (!IsValid(ControlledCharacter)) return false;

    // Get the player's current grid position
    FVector CurrentLocation = ControlledCharacter->GetActorLocation();
    int PlayerX = FMath::RoundToInt(CurrentLocation.X / CellSize);
    int PlayerY = FMath::RoundToInt(CurrentLocation.Y / CellSize);

    // Calculate Manhattan distance
    int DistanceX = FMath::Abs(PlayerX - CellIndex.X);
    int DistanceY = FMath::Abs(PlayerY - CellIndex.Y);

    bool bIsWithinRange = (DistanceX + DistanceY) <= ControlledCharacter->GetMaxMoveRange();

    // If the tile is out of range, return false
    if (!bIsWithinRange)
    {
        return false;
    }

    // If you have obstacles, check if the tile is occupied
    FHitResult HitResult;
    FVector TileWorldPosition = FVector(CellIndex.X * CellSize, CellIndex.Y * CellSize, ControlledCharacter->GetActorLocation().Z);

    if (GetWorld()->LineTraceSingleByChannel(HitResult, TileWorldPosition + FVector(0, 0, 50), TileWorldPosition - FVector(0, 0, 50), ECC_Visibility))
    {
        AActor* HitActor = HitResult.GetActor();
        if (HitActor != nullptr && HitActor->ActorHasTag("BlockedTile"))
        {
            return false; // Tile is blocked
        }
    }

    return true; // Otherwise, the cell is valid for movement
}

