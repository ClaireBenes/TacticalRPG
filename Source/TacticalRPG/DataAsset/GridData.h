#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GridData.generated.h"

/**
 * 
 */
UCLASS()
class TACTICALRPG_API UGridData : public UDataAsset
{
	GENERATED_BODY()
	

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Size")
	int GridSizeX = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Size")
	int GridSizeY = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CellSize")
	float CellSize = 100.0f;
};
