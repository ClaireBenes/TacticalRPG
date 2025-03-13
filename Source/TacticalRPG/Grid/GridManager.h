#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GridManager.generated.h"

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

    // Check if a given grid cell is valid for movement
    bool IsCellInRange(FVector2D CellIndex);

private:
    void GenerateGrid();
    void UpdateGridPosition();
    void UpdateHoveredCell();

    UPROPERTY(EditAnywhere, Category = "Grid Settings")
    int GridSizeX = 10;

    UPROPERTY(EditAnywhere, Category = "Grid Settings")
    int GridSizeY = 10;

    UPROPERTY(EditAnywhere, Category = "Grid Settings")
    float CellSize = 100.0f;

    UPROPERTY(EditAnywhere, Category = "Grid Settings")
    int MaxMoveRange = 3;

    UPROPERTY()
    APlayerController* PlayerController;

    UPROPERTY()
    ACharacter* PlayerCharacter;

    UPROPERTY()
    UMaterialInstanceDynamic* HoverMaterial;

    UPROPERTY()
    AActor* HoveredCell;

};
