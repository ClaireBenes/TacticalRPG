#include "TacticalRPG/Player/PlayerCharacterController.h"

#include "TacticalRPG/Player/PlayerCharacter.h"
#include "TacticalRPG/Player/CameraPawn.h"
#include "TacticalRPG/Grid/GridManager.h"
#include "TacticalRPG/DataAsset/CameraData.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PlayerController.h"
#include "Components/ArrowComponent.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"

#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"

APlayerCharacterController::APlayerCharacterController()
{
	bShowMouseCursor = true;
}

void APlayerCharacterController::BeginPlay()
{
    Super::BeginPlay();

    // Get the controlled PlayerCharacter
    AActor* FoundActor = UGameplayStatics::GetActorOfClass(GetWorld(), APlayerCharacter::StaticClass());
    ControlledCharacter = Cast<APlayerCharacter>(FoundActor);

    CameraPawn = Cast<ACameraPawn>(GetPawn());

    // Find GridManager in the world
    for (TActorIterator<AGridManager> It(GetWorld()); It; ++It)
    {
        GridManager = *It;
        break;
    }

    // Add default input mapping context
    if (UEnhancedInputLocalPlayerSubsystem* InputSubsystem = GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
    {
        if (DefaultInputMappingContext)
        {
            InputSubsystem->AddMappingContext(DefaultInputMappingContext, 0);
        }
    }
}

void APlayerCharacterController::SetupInputComponent()
{
    Super::SetupInputComponent();

    UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);

    if (ClickInputAction)
    {
        EnhancedInputComponent->BindAction(ClickInputAction, ETriggerEvent::Started, this, &APlayerCharacterController::Click);
    }

    if (ZoomInputAction)
    {
        EnhancedInputComponent->BindAction(ZoomInputAction, ETriggerEvent::Triggered, this, &APlayerCharacterController::Zoom);
    }

    if (MoveCameraInputAction)
    {
        EnhancedInputComponent->BindAction(MoveCameraInputAction, ETriggerEvent::Triggered, this, &APlayerCharacterController::MoveCamera);
    }

    if (GyroscopeCameraInputAction)
    {
        EnhancedInputComponent->BindAction(GyroscopeCameraInputAction, ETriggerEvent::Triggered, this, &APlayerCharacterController::RotateCamera);
    }
}

void APlayerCharacterController::MoveCharacter()
{
    if (!IsValid(ControlledCharacter) || !IsValid(GridManager)) return;

    // Prevent movement if character is already moving
    if (ControlledCharacter->IsCharacterMoving())
    {
        UE_LOG(LogTemp, Warning, TEXT("Can't move: Character is still moving!"));
        return;
    }

    // Get mouse position in the world
    FVector WorldLocation, WorldDirection;
    if (DeprojectMousePositionToWorld(WorldLocation, WorldDirection))
    {
        FVector Start = WorldLocation;
        FVector End = Start + (WorldDirection * 10000);

        FHitResult HitResult;
        if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility))
        {
            FVector ClickedLocation = HitResult.Location;

            int ClickedCellX = FMath::RoundToInt(ClickedLocation.X / ControlledCharacter->GetGridSize());
            int ClickedCellY = FMath::RoundToInt(ClickedLocation.Y / ControlledCharacter->GetGridSize());
            FVector2D ClickedCell(ClickedCellX, ClickedCellY);

            if (GridManager->IsCellInRange(ClickedCell))
            {
                FVector NextTargetLocation = FVector(ClickedCellX * ControlledCharacter->GetGridSize(),
                    ClickedCellY * ControlledCharacter->GetGridSize(),
                    ControlledCharacter->GetActorLocation().Z);

                GridManager->HideMovementGrid();

                ControlledCharacter->MoveToGridCell(NextTargetLocation);
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("Invalid Move: This tile is out of range!"));
            }
        }
    }
}

void APlayerCharacterController::Click()
{
    if (HoveredCharacter != nullptr)
    {
        ControlledCharacter = HoveredCharacter;
        GridManager->ControlledCharacter = ControlledCharacter;
        GridManager->UpdateGridPosition();
    }
    else
    {
        MoveCharacter();
    }
}

void APlayerCharacterController::Zoom(const FInputActionValue& Value)
{
    if (!IsValid(CameraPawn)) return;

    float MouseWheelValue = Value.Get<float>();

    float ClampInValue = (CameraPawn->DesiredArmLenght + ((MouseWheelValue * (-1)) * CameraData->ZoomInputStep));
    float ClampOutValue = FMath::Clamp(ClampInValue, CameraData->MinZoom, CameraData->MaxZoom);

    CameraPawn->DesiredArmLenght = ClampOutValue;
}

void APlayerCharacterController::MoveCamera(const FInputActionValue& Value)
{
    if (!IsValid(CameraPawn)) return;

    FVector2D Values = Value.Get<FVector2D>();

    CameraPawn->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

    FVector SideWorldDirection = CameraPawn->TopDownCamera->GetRightVector();
    float SideScaleValues = Values.X * CameraData->SideSpeed;

    CameraPawn->AddMovementInput(SideWorldDirection, SideScaleValues);

    FVector ForwardWorldDirection = CameraPawn->ForwardArrow->GetForwardVector();
    float ForwardScaleValues = Values.Y * CameraData->ForwardSpeed;

    CameraPawn->AddMovementInput(ForwardWorldDirection, ForwardScaleValues);
}

void APlayerCharacterController::RotateCamera()
{
    if (!IsValid(CameraPawn)) return;

    FRotator SpringArmRotation = CameraPawn->SpringArm->GetComponentRotation();
    float RotX = SpringArmRotation.Roll;

    float X = 0.0f, Y = 0.0f;
    GetInputMouseDelta(X, Y);

    // Prevent teleporting by ensuring there's actual input
    if (FMath::IsNearlyZero(X) && FMath::IsNearlyZero(Y)) return;

    float Speed = CameraData->SpeedGyroscope;

    float ClampInValue = (Y * Speed) + SpringArmRotation.Pitch;
    float RotY = FMath::Clamp(ClampInValue, CameraData->MinYRotation, CameraData->MaxYRotation);

    float RotZ = (X * Speed) + SpringArmRotation.Yaw;

    CameraPawn->SpringArm->SetWorldRotation(FRotator(RotY, RotZ, RotX));

    float SpringArmRotZ = CameraPawn->SpringArm->GetComponentRotation().Yaw;
    CameraPawn->ForwardArrow->SetWorldRotation(FRotator(0.0f, SpringArmRotZ, 0.0f));
}
