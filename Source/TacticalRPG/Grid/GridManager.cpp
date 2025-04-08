#include "TacticalRPG/Grid/GridManager.h"

#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

#include "TacticalRPG/Utility/Pathfinding.h"

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

    CacheObstacles(); // Detect obstacles at game start
}

// Called every frame
void AGridManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
    UpdateGridPosition();
	UpdateHoveredCell();
 
    //for (FVector2D Cell : ObstacleCells)
    //{
    //    FVector WorldPos = FVector(Cell.X * GridData->CellSize, Cell.Y * GridData->CellSize, 50); // Adjust Z if needed
    //    DrawDebugBox(GetWorld(), WorldPos, FVector(GridData->CellSize / 2, GridData->CellSize / 2, 50), FColor::Red, false, 5.0f);
    //}
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

void AGridManager::CacheObstacles()
{
    TArray<AActor*> ObstacleActors;
    UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Obstacle"), ObstacleActors);

    TArray<AActor*> CharacterActors;
    UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Character"), CharacterActors);

    // Merge both lists
    ObstacleCells.Empty();

    for (AActor* Actor : CharacterActors)
    {
        FVector2D Cell = ConvertWorldToGrid(Actor->GetActorLocation());
        ObstacleCells.Add(Cell);
    }

    for (AActor* Obstacle : ObstacleActors)
    {
        if (!Obstacle) continue;

        // Get the full bounds of the obstacle
        FBox Bounds = Obstacle->GetComponentsBoundingBox();
        FVector Min = Bounds.Min;
        FVector Max = Bounds.Max;

        // Convert the bounding box to grid coordinates
        FVector2D MinGridCoord = ConvertWorldToGrid(Min);
        FVector2D MaxGridCoord = ConvertWorldToGrid(Max);

        // Loop over all grid cells covered by the obstacle
        for (int x = MinGridCoord.X; x <= MaxGridCoord.X; x++)
        {
            for (int y = MinGridCoord.Y; y <= MaxGridCoord.Y; y++)
            {
                FVector2D GridCoord(x, y);
                ObstacleCells.Add(GridCoord); // Mark cell as blocked
            }
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
    HideMovementGrid();

    // Draw the grid centered around the player
    for (int X = -GridData->GridSizeX / 2; X <= GridData->GridSizeX / 2; X++)
    {
        for (int Y = -GridData->GridSizeY / 2; Y <= GridData->GridSizeY / 2; Y++)
        {
            FVector CellLocation = PlayerLocation + FVector(X * GridData->CellSize, Y * GridData->CellSize, 0);

            FVector2D CenterCell = ConvertWorldToGrid(PlayerLocation);
            FVector2D CellIndex = FVector2D(CenterCell.X + X, CenterCell.Y + Y);

            // Create an invisible actor for tiles that can be moved onto
            if (IsCellInRange(CellIndex))
            {
                if (!GridCharacterMap.Contains(CellIndex) && !ObstacleCells.Contains(CellIndex))
                {
                    ValidCells.Add(CellIndex); // Store only valid movement tiles
                }
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
            DrawDebugLine(GetWorld(), TopRight, BottomRight, GridData->ValidCellColor, true, -1, 0, GridData->OutlineSize);
        }
        if (!ValidCells.Contains(Neighbors[1])) // Left Edge
        {
            DrawDebugLine(GetWorld(), TopLeft, BottomLeft, GridData->ValidCellColor, true, -1, 0, GridData->OutlineSize);
        }
        if (!ValidCells.Contains(Neighbors[2])) // Top Edge
        {
            DrawDebugLine(GetWorld(), TopLeft, TopRight, GridData->ValidCellColor, true, -1, 0, GridData->OutlineSize);
        }
        if (!ValidCells.Contains(Neighbors[3])) // Bottom Edge
        {
            DrawDebugLine(GetWorld(), BottomLeft, BottomRight, GridData->ValidCellColor, true, -1, 0, GridData->OutlineSize);
        }
    }

    // Store new cell pos of character
    FVector2D NewCellIndex = ConvertWorldToGrid(ControlledCharacter->GetActorLocation());

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
    CacheObstacles();
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

            FVector2D CellIndex = ConvertWorldToGrid(HitLocation);

            APlayerCharacterController* PlayerCharacterController = Cast<APlayerCharacterController>(PlayerController);
            PlayerCharacterController->HoveredCharacter = nullptr;

            bool bCanClick = false;
             //Direct hit detection (if mouse is over character)
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


            //FVector TopRight = CellWorldLocation + FVector(GridData->CellSize / 2, GridData->CellSize / 2, 0);
            //FVector BottomLeft = CellWorldLocation + FVector(-GridData->CellSize / 2, -GridData->CellSize / 2, 0);
            //FVector BottomRight = CellWorldLocation + FVector(GridData->CellSize / 2, -GridData->CellSize / 2, 0);


            // Draw the hover outline (Green = Valid, Red = Invalid)
            FVector CellWorldPos = ConvertGridToWorld(CellIndex) + FVector(0, 0, 5);
            DrawDebugBox(GetWorld(), CellWorldPos, FVector(GridData->CellSize / 2, -GridData->CellSize / 2, 0),
                bCanClick ? GridData->ValidCellColor : GridData->WrongCellColor, false, -1, 0, GridData->OutlineSize);


            DrawDebugLine(GetWorld(), Start, HitLocation, FColor::Green, false, -1.0f);
            DrawDebugPoint(GetWorld(), HitLocation, 5.0f, FColor::Green, false, -1.0f);
        }
        else
        {
            DrawDebugLine(GetWorld(), Start, End, GridData->WrongCellColor, false, -1.0f);
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

TArray<FVector2D> AGridManager::FindPathToCell(FVector2D Start, FVector2D Goal)
{
    if (!Pathfinding)
    {
        Pathfinding = GetWorld()->SpawnActor<APathfinding>();
    }

    TArray<FVector2D> Path = Pathfinding->FindPath(Start, Goal, ObstacleCells, GridData->GridSizeX, GridData->GridSizeY);

    return Path;
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

    return true; // Otherwise, the cell is valid for movement
}

TSet<FVector2D> AGridManager::GetValidCells()
{
    return ValidCells;
}

FVector2D AGridManager::ConvertWorldToGrid(FVector WorldLocation) const
{
    int CellX = FMath::RoundToInt(WorldLocation.X / GridData->CellSize);
    int CellY = FMath::RoundToInt(WorldLocation.Y / GridData->CellSize);

    return FVector2D(CellX, CellY);
}

FVector AGridManager::ConvertGridToWorld(FVector2D GridCoord) const
{
    float WorldX = GridCoord.X * GridData->CellSize;
    float WorldY = GridCoord.Y * GridData->CellSize;

    return FVector(WorldX, WorldY, 0); // Z = 0 assuming flat terrain
}

