#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GridBorderVisualizer.generated.h"

class UGridData;

UCLASS()
class TACTICALRPG_API AGridBorderVisualizer : public AActor
{
	GENERATED_BODY()
	
public:	
	AGridBorderVisualizer();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid Border")
    UStaticMesh* BorderMesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid Border")
    UMaterialInterface* BorderMaterial;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data Asset")
    UGridData* GridData = nullptr;

    //UFUNCTION(BlueprintCallable)
    void DrawGridBorders(const TSet<FVector2D>& ValidCells, float CellSize);

    UFUNCTION(BlueprintCallable)
    void ClearBorders();

protected:
	virtual void OnConstruction(const FTransform& Transform) override;

private:

    void AddBorderMesh(FVector Location, FRotator Rotation, FVector Scale);

    UPROPERTY()
    TArray<UStaticMeshComponent*> BorderMeshComponents;
};
