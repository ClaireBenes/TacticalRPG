#pragma once

#include "TacticalRPG/DataAsset/GridData.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GridManager.generated.h"

class APlayerCharacter;
class UCameraData;
class APathfinding;

UCLASS()
class TACTICALRPG_API AGridManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGridManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

    UFUNCTION(BlueprintCallable, Category = "Grid")
    void HideMovementGrid();
    UFUNCTION(BlueprintCallable, Category = "Grid")
    void UpdateGridPosition();

    UFUNCTION(BlueprintCallable, Category = "Grid")
    FVector2D ConvertWorldToGrid(FVector WorldLocation) const;
    UFUNCTION(BlueprintCallable, Category = "Grid")
    FVector ConvertGridToWorld(FVector2D GridCoord) const;

    // Check if a given grid cell is valid for movement
    TArray<FVector2D> FindPathToCell(FVector2D Start, FVector2D Goal);
    bool IsCellInRange(FVector2D CellIndex);

    //Getters
    float GetGridSize() const { return GridData->CellSize; }
    TSet<FVector2D> GetValidCells();


public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data Asset")
    UCameraData* CameraData = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data Asset")
    UGridData* GridData = nullptr;

    UPROPERTY()
    APlayerCharacter* ControlledCharacter = nullptr;

private:
    void GenerateGrid();
    void UpdateHoveredCell();
    void InitializeCharacterPositions();

    void CacheObstacles(); // Detect obstacles at game start

    UPROPERTY()
    APlayerController* PlayerController = nullptr;

    UPROPERTY()
    UMaterialInstanceDynamic* HoverMaterial = nullptr;

    UPROPERTY()
    AActor* HoveredCell = nullptr;

    APathfinding* Pathfinding = nullptr;

    TSet<FVector2D> ObstacleCells; // Stores obstacles in grid coordinates
    TSet<FVector2D> ValidCells; // Stores all valid movement cells
    TMap<FVector2D, APlayerCharacter*> GridCharacterMap; //Store all cells where character are standing on
};
