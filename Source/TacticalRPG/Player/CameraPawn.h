#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "CameraPawn.generated.h"

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
	class UCameraData* CameraData = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Camera")
	class UCameraComponent* TopDownCamera = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Camera|SpringArm")
	class USpringArmComponent* SpringArm = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|SpringArm")
	float DesiredArmLenght = 100.0f;

private:
	UPROPERTY(VisibleAnywhere, Category = "Movement")
	class UFloatingPawnMovement* FloatingPawnMovement = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Root")
	class UStaticMeshComponent* NewRootComponent = nullptr;	
};
