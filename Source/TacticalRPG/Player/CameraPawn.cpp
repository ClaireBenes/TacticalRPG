#include "TacticalRPG/Player/CameraPawn.h"

#include "GameFramework/FloatingPawnMovement.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/ArrowComponent.h"

#include "TacticalRPG/DataAsset/CameraData.h"

// Sets default values
ACameraPawn::ACameraPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Create Root Component
	NewRootComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Root"));
	NewRootComponent->SetupAttachment(RootComponent);

	// Create the Arrow
	ForwardArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	ForwardArrow->SetupAttachment(NewRootComponent);

	// Create the SpringArm
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(NewRootComponent);
	SpringArm->bDoCollisionTest = false; // Prevent the camera from clipping through walls

	// Create the Camera
	TopDownCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCamera->SetupAttachment(SpringArm);
	TopDownCamera->bUsePawnControlRotation = false; // Prevent the camera from rotating with the character

	// Create the FloatingPawnMovement
	FloatingPawnMovement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("FloatingPawnMovement"));
}

// Called when the game starts or when spawned
void ACameraPawn::BeginPlay()
{
	Super::BeginPlay();
	
	DesiredArmLenght = SpringArm->TargetArmLength;
}

// Called every frame
void ACameraPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float newAlpha = CameraData->ZoomSmoothSpeed * DeltaTime;
	SpringArm->TargetArmLength = FMath::Lerp(SpringArm->TargetArmLength, DesiredArmLenght, newAlpha);
}

// Called to bind functionality to input
void ACameraPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

