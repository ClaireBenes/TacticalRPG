#pragma once

#include "TacticalRPG/DataAsset/GridData.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GridManager.generated.h"

class APlayerCharacter;
class UCameraData;

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

    // Check if a given grid cell is valid for movement
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

    UPROPERTY()
    APlayerController* PlayerController;

    UPROPERTY()
    UMaterialInstanceDynamic* HoverMaterial;

    UPROPERTY()
    AActor* HoveredCell;

    TSet<FVector2D> ValidCells; // Stores all valid movement cells
    TMap<FVector2D, APlayerCharacter*> GridCharacterMap; //Store all cells where character are standing on
};
