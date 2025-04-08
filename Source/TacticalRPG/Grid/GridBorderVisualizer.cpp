#include "TacticalRPG/Grid/GridBorderVisualizer.h"

#include "TacticalRPG/DataAsset/GridData.h"

// Sets default values
AGridBorderVisualizer::AGridBorderVisualizer()
{
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
}

void AGridBorderVisualizer::DrawGridBorders(const TSet<FVector2D>& ValidCells, float CellSize)
{
    ClearBorders();

    // Identify Edge Cells
// Identify Edge Cells
    for (const FVector2D& Cell : ValidCells)
    {
        FVector CellWorldLocation = FVector(Cell.X * GridData->CellSize, Cell.Y * GridData->CellSize, 5);
        FVector TopLeft = CellWorldLocation + FVector(-GridData->CellSize / 2, GridData->CellSize / 2, 0);
        FVector TopRight = CellWorldLocation + FVector(GridData->CellSize / 2, GridData->CellSize / 2, 0);
        FVector BottomLeft = CellWorldLocation + FVector(-GridData->CellSize / 2, -GridData->CellSize / 2, 0);
        FVector BottomRight = CellWorldLocation + FVector(GridData->CellSize / 2, -GridData->CellSize / 2, 0);

        // Check adjacent tiles
        FVector2D Neighbors[4] = {
            FVector2D(Cell.X + 1, Cell.Y),  // Right
            FVector2D(Cell.X - 1, Cell.Y),  // Left
            FVector2D(Cell.X, Cell.Y + 1),  // Top
            FVector2D(Cell.X, Cell.Y - 1)   // Bottom
        };

        // Right Edge
        if (!ValidCells.Contains(Neighbors[0]))
        {
            FVector Pos = TopRight + FVector(GridData->CellSize * 0.5f, 0, 0);
            AddBorderMesh(Pos, FRotator(0, 90, 0), FVector(GridData->CellSize * 0.5f, 5.0f, 5.0f)); // Right border
        }

        // Left Edge
        if (!ValidCells.Contains(Neighbors[1]))
        {
            FVector Pos = TopLeft - FVector(GridData->CellSize * 0.5f, 0, 0);
            AddBorderMesh(Pos, FRotator(0, 90, 0), FVector(GridData->CellSize * 0.5f, 5.0f, 5.0f)); // Left border
        }

        // Top Edge
        if (!ValidCells.Contains(Neighbors[2]))
        {
            FVector Pos = TopLeft + FVector(0, GridData->CellSize * 0.5f, 0);
            AddBorderMesh(Pos, FRotator::ZeroRotator, FVector(GridData->CellSize * 0.5f, 5.0f, 5.0f)); // Top border
        }

        // Bottom Edge
        if (!ValidCells.Contains(Neighbors[3]))
        {
            FVector Pos = BottomLeft - FVector(0, GridData->CellSize * 0.5f, 0);
            AddBorderMesh(Pos, FRotator::ZeroRotator, FVector(GridData->CellSize * 0.5f, 5.0f, 5.0f)); // Bottom border
        }
    }
}


void AGridBorderVisualizer::OnConstruction(const FTransform& Transform)
{
    ClearBorders();
}

void AGridBorderVisualizer::ClearBorders()
{
    for (auto* Comp : BorderMeshComponents)
    {
        if (Comp)
        {
            Comp->DestroyComponent();
        }
    }
    BorderMeshComponents.Empty();
}

void AGridBorderVisualizer::AddBorderMesh(FVector Location, FRotator Rotation, FVector Scale)
{
    if (!BorderMesh || !BorderMaterial) return;

    UStaticMeshComponent* MeshComp = NewObject<UStaticMeshComponent>(this);
    MeshComp->RegisterComponent();
    MeshComp->SetStaticMesh(BorderMesh);
    MeshComp->SetMaterial(0, BorderMaterial);
    MeshComp->SetWorldLocation(Location);
    MeshComp->SetWorldRotation(Rotation);
    MeshComp->SetWorldScale3D(Scale);
    MeshComp->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);

    BorderMeshComponents.Add(MeshComp);
}

