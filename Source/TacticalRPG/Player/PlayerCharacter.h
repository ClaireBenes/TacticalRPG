#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

UCLASS()
class TACTICALRPG_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnClickMove(); // Called when clicking on the grid

	void MoveToGridCell(FVector NewTargetLocation);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	UPROPERTY(VisibleAnywhere, Category = "Camera")
	class USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, Category = "Camera")
	class UCameraComponent* TopDownCamera;

	// Grid square size
	UPROPERTY(EditAnywhere, Category = "Grid")
	float GridSize = 100.0f; 

	// Max number of squares per turn
	UPROPERTY(EditAnywhere, Category = "Grid")
	int MaxMoveRange = 3; 

	FVector TargetLocation;
	bool bIsMoving = false;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float MoveSpeed = 300.0f; // Units per second

	APlayerController* PlayerController;
	class AGridManager* GridManager;
};
