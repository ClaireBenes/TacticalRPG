#include "TacticalRPG/Grid/GridManager.h"

#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

#include "TacticalRPG/Player/PlayerCharacter.h"
#include "TacticalRPG/Player/PlayerCharacterController.h"
#include "TacticalRPG/DataAsset/CameraData.h"
#include "TacticalRPG/DataAsset/GridData.h"

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
	GenerateGrid();
    InitializeCharacterPositions();
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
	for (int X = 0; X < GridData->GridSizeX; X++)
	{
		for (int Y = 0; Y < GridData->GridSizeY; Y++)
		{
			FVector CellLocation = FVector(X * GridData->CellSize, Y * GridData->CellSize, 0);
		}
	}
}

void AGridManager::UpdateGridPosition()
{
    if (!IsValid(ControlledCharacter) || ControlledCharacter->IsCharacterMoving())
    {
        return; // Do NOT update grid if character is moving
    }

    // VISUAL DEBUG
    FVector PlayerLocation = ControlledCharacter->GetActorLocation();
    PlayerLocation.X = FMath::RoundToInt(PlayerLocation.X / GridData->CellSize) * GridData->CellSize;
    PlayerLocation.Y = FMath::RoundToInt(PlayerLocation.Y / GridData->CellSize) * GridData->CellSize;
    PlayerLocation.Z = 0;

    // Clear previous debug grid
    FlushPersistentDebugLines(GetWorld());
    ValidCells.Empty();

    // Draw the grid centered around the player
    for (int X = -GridData->GridSizeX / 2; X <= GridData->GridSizeX / 2; X++)
    {
        for (int Y = -GridData->GridSizeY / 2; Y <= GridData->GridSizeY / 2; Y++)
        {
            FVector CellLocation = PlayerLocation + FVector(X * GridData->CellSize, Y * GridData->CellSize, 0);
            FVector2D CellIndex = FVector2D(X + (PlayerLocation.X / GridData->CellSize), Y + (PlayerLocation.Y / GridData->CellSize));

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
        FVector CellWorldLocation = FVector(Cell.X * GridData->CellSize, Cell.Y * GridData->CellSize, 5);
        FVector TopLeft = CellWorldLocation + FVector(-GridData->CellSize / 2, GridData->CellSize / 2, 0);
        FVector TopRight = CellWorldLocation + FVector(GridData->CellSize / 2, GridData->CellSize / 2, 0);
        FVector BottomLeft = CellWorldLocation + FVector(-GridData->CellSize / 2, -GridData->CellSize / 2, 0);
        FVector BottomRight = CellWorldLocation + FVector(GridData->CellSize / 2, -GridData->CellSize / 2, 0);

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

    // Store new cell pos of character
    int CellX = FMath::RoundToInt(ControlledCharacter->GetActorLocation().X / GridData->CellSize);
    int CellY = FMath::RoundToInt(ControlledCharacter->GetActorLocation().Y / GridData->CellSize);
    FVector2D NewCellIndex(CellX, CellY);

    // Remove character from previous cell
    for (auto It = GridCharacterMap.CreateIterator(); It; ++It)
    {
        if (It.Value() == ControlledCharacter)
        {
            It.RemoveCurrent();
            break;
        }
    }

    // Add character to new cell
    GridCharacterMap.Add(NewCellIndex, ControlledCharacter);
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
        FVector End = Start + (WorldDirection * CameraData->RaycastLenght);

        FHitResult HitResult;
        if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility))
        {
            FVector HitLocation = HitResult.Location;
            AActor* HitActor = HitResult.GetActor();

            int CellX = FMath::RoundToInt(HitLocation.X / GridData->CellSize);
            int CellY = FMath::RoundToInt(HitLocation.Y / GridData->CellSize);
            FVector2D CellIndex(CellX, CellY);

            bool bCanClick = false;
            APlayerCharacterController* PlayerCharacterController = Cast<APlayerCharacterController>(PlayerController);
            PlayerCharacterController->HoveredCharacter = nullptr;

            // Direct hit detection (if mouse is over character)
            if (IsValid(PlayerCharacterController))
            {
                if (HitActor != nullptr && Cast<APlayerCharacter>(HitActor))
                {
                    PlayerCharacterController->HoveredCharacter = Cast<APlayerCharacter>(HitActor);
                    bCanClick = true;
                }
                else if (GridCharacterMap.Contains(CellIndex))
                {
                    PlayerCharacterController->HoveredCharacter = GridCharacterMap[CellIndex];
                    bCanClick = true;
                }
                else if (ValidCells.Contains(CellIndex))
                {
                    bCanClick = true;
                }
            }

            // Draw the hover outline (Green = Valid, Red = Invalid)
            DrawDebugBox(GetWorld(), FVector(CellX * GridData->CellSize, CellY * GridData->CellSize, 5), FVector(GridData->CellSize / 2, GridData->CellSize / 2, 5),
                bCanClick ? FColor::Green : FColor::Red, false, -1, 0, 5);

            DrawDebugLine(GetWorld(), Start, HitLocation, FColor::Green, false, -1.0f);
            DrawDebugPoint(GetWorld(), HitLocation, 5.0f, FColor::Green, false, -1.0f);
        }
        else
        {
            DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, -1.0f);
        }
    }
}

void AGridManager::InitializeCharacterPositions()
{
    GridCharacterMap.Empty(); // Clear any previous data

    for (TActorIterator<APlayerCharacter> It(GetWorld()); It; ++It)
    {
        APlayerCharacter* Character = *It;
        if (IsValid(Character))
        {
            int CellX = FMath::RoundToInt(Character->GetActorLocation().X / GridData->CellSize);
            int CellY = FMath::RoundToInt(Character->GetActorLocation().Y / GridData->CellSize);
            FVector2D CellIndex(CellX, CellY);

            GridCharacterMap.Add(CellIndex, Character);
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
    int PlayerX = FMath::RoundToInt(CurrentLocation.X / GridData->CellSize);
    int PlayerY = FMath::RoundToInt(CurrentLocation.Y / GridData->CellSize);

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
    FVector TileWorldPosition = FVector(CellIndex.X * GridData->CellSize, CellIndex.Y * GridData->CellSize, ControlledCharacter->GetActorLocation().Z);

    //FVector Start = WorldLocation;
    //FVector End = Start + (WorldDirection * CameraData->RaycastLenght);
    if (GetWorld()->LineTraceSingleByChannel(HitResult, TileWorldPosition + FVector(0, 0, 50), TileWorldPosition - FVector(0, 0, 50), ECC_Visibility))
    {
        AActor* HitActor = HitResult.GetActor();
        if (HitActor != nullptr && HitActor->ActorHasTag("Obstacle"))
        {
            return false; // Tile is blocked
        }
    }

    return true; // Otherwise, the cell is valid for movement
}

TSet<FVector2D> AGridManager::GetValidCells()
{
    return ValidCells;
}

