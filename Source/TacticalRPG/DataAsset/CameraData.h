#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CameraData.generated.h"

/**
 * 
 */
UCLASS()
class TACTICALRPG_API UCameraData : public UDataAsset
{
	GENERATED_BODY()
	
public:
	// Zoom
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoom")
	float MinZoom = 425.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoom")
	float MaxZoom = 3000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoom")
	float ZoomInputStep = 175.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoom")
	float ZoomSmoothSpeed = 2.0f;

	// Gyroscope
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gyroscope")
	float SpeedGyroscope = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gyroscope")
	float MinYRotation = -85.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gyroscope")
	float MaxYRotation = -2.0f;

	// Focus On Character
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Focus On Character")
	float MoveToCharacterSpeed = 0.5f;

	// Move Speed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Move Speed")
	float SideSpeed = 1000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Move Speed")
	float ForwardSpeed = 1000.0f;

	//RayCast
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Raycast Clickable Lenght")
	float RaycastLenght = 5000.0f;
};
