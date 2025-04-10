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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Outline | Color")
	FColor ValidWhiteCellColor = FColor::Black;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Outline | Color")
	FColor ValidBlackCellColor = FColor::Black;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Outline | Color")
	FColor WrongCellColor = FColor::White;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Outline")
	float OutlineSize = 1.0f;
};
