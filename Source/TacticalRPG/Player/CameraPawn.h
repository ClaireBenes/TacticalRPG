#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "CameraPawn.generated.h"

class UCameraData;
class UArrowComponent;
class UCameraComponent;
class USpringArmComponent;
class UFloatingPawnMovement;
class UStaticMeshComponent;

UCLASS()
class TACTICALRPG_API ACameraPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ACameraPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data Asset")
	UCameraData* CameraData = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Camera")
	UArrowComponent* ForwardArrow = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Camera")
	UCameraComponent* TopDownCamera = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Camera|SpringArm")
	USpringArmComponent* SpringArm = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|SpringArm")
	float DesiredArmLenght = 100.0f;

private:
	UPROPERTY(VisibleAnywhere, Category = "Movement")
	UFloatingPawnMovement* FloatingPawnMovement = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Root")
	UStaticMeshComponent* NewRootComponent = nullptr;	
};
