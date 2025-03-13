#include "TacticalRPG/Grid/GridManager.h"

#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

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
    PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
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
    if (!IsValid(PlayerCharacter)) return;

    FVector PlayerLocation = PlayerCharacter->GetActorLocation();
    PlayerLocation.X = FMath::RoundToInt(PlayerLocation.X / CellSize) * CellSize;
    PlayerLocation.Y = FMath::RoundToInt(PlayerLocation.Y / CellSize) * CellSize;
    PlayerLocation.Z = 0;

    // Clear previous debug grid
    FlushPersistentDebugLines(GetWorld());

    // Draw the grid centered around the player
    for (int X = -GridSizeX / 2; X <= GridSizeX / 2; X++)
    {
        for (int Y = -GridSizeY / 2; Y <= GridSizeY / 2; Y++)
        {
            FVector CellLocation = PlayerLocation + FVector(X * CellSize, Y * CellSize, 0);
            FVector2D CellIndex = FVector2D(X + (PlayerLocation.X / CellSize), Y + (PlayerLocation.Y / CellSize));

            // Check if it's within movement range
            bool bCanMove = IsCellInRange(CellIndex);

            // Draw cell outline
            //DrawDebugBox(GetWorld(), CellLocation, FVector(CellSize / 2, CellSize / 2, 5), bCanMove ? FColor::Green : FColor::Red, true, -1, 0, 2);

            // Create an invisible actor for tiles that can be moved onto
            if (bCanMove)
            {
                AActor* Tile = GetWorld()->SpawnActor<AActor>(AActor::StaticClass(), CellLocation, FRotator::ZeroRotator);
                if (Tile)
                {
                    Tile->Tags.Add("ValidTile"); // Mark this as a valid tile
                }
            }
        }
    }
}

void AGridManager::UpdateHoveredCell()
{
    if (!IsValid(PlayerController)) return;

    FVector WorldLocation, WorldDirection;
    if (PlayerController->DeprojectMousePositionToWorld(WorldLocation, WorldDirection))
    {
        FVector Start = WorldLocation;
        FVector End = Start + (WorldDirection * 10000);

        FHitResult HitResult;
        if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility))
        {
            FVector HitLocation = HitResult.Location;

            int CellX = FMath::RoundToInt(HitLocation.X / CellSize);
            int CellY = FMath::RoundToInt(HitLocation.Y / CellSize);

            FVector2D CellIndex(CellX, CellY);

            // Check if cell is in move range
            bool bCanMove = IsCellInRange(CellIndex);

            // Draw the hover outline (Green = Valid, Red = Invalid)
            DrawDebugBox(GetWorld(), FVector(CellX * CellSize, CellY * CellSize, 5), FVector(CellSize / 2, CellSize / 2, 5), bCanMove ? FColor::Green : FColor::Red, false, -1, 0, 5);
        }
    }
}

bool AGridManager::IsCellInRange(FVector2D CellIndex)
{
    // Get character position
    if (!IsValid(PlayerCharacter)) return false;

    // Get the player's current grid position
    FVector CurrentLocation = PlayerCharacter->GetActorLocation();
    int PlayerX = FMath::RoundToInt(CurrentLocation.X / CellSize);
    int PlayerY = FMath::RoundToInt(CurrentLocation.Y / CellSize);

    // Calculate Manhattan distance
    int DistanceX = FMath::Abs(PlayerX - CellIndex.X);
    int DistanceY = FMath::Abs(PlayerY - CellIndex.Y);

    bool bIsWithinRange = (DistanceX + DistanceY) <= MaxMoveRange;

    // If the tile is out of range, return false
    if (!bIsWithinRange)
    {
        return false;
    }

    // OPTIONAL: If you have obstacles, check if the tile is occupied
    FHitResult HitResult;
    FVector TileWorldPosition = FVector(CellIndex.X * CellSize, CellIndex.Y * CellSize, PlayerCharacter->GetActorLocation().Z);

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

