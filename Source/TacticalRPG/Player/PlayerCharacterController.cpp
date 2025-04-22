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

#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"

#include "TacticalRPG/Enemy/EnemyCharacter.h"

APlayerCharacterController::APlayerCharacterController()
{
	bShowMouseCursor = true;
}

void APlayerCharacterController::BeginPlay()
{
    Super::BeginPlay();

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

    AActor* FoundActor = UGameplayStatics::GetActorOfClass(GetWorld(), APostProcessVolume::StaticClass());
    PostProcessVolumeRef = Cast<APostProcessVolume>(FoundActor);
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
        FVector End = Start + (WorldDirection * CameraData->RaycastLenght);

        FHitResult HitResult;
        if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility))
        {
            FVector ClickedLocation = HitResult.Location;

            int ClickedCellX = FMath::RoundToInt(ClickedLocation.X / GridManager->GetGridSize());
            int ClickedCellY = FMath::RoundToInt(ClickedLocation.Y / GridManager->GetGridSize());
            FVector2D ClickedCell(ClickedCellX, ClickedCellY);

            if (GridManager->GetValidCells().Contains(ClickedCell))
            {
                FVector NextTargetLocation = FVector(ClickedCellX * GridManager->GetGridSize(),
                    ClickedCellY * GridManager->GetGridSize(),
                    ControlledCharacter->GetActorLocation().Z);

                GridManager->HideMovementGrid();

                FVector2D StartCell = GridManager->ConvertWorldToGrid(ControlledCharacter->GetActorLocation());

                TArray<FVector2D> Path = GridManager->FindPathToCell(StartCell, ClickedCell);
                if (Path.Num() > 0)
                {
                    ControlledCharacter->SetPath(Path);
                }
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("Invalid Move: This tile is out of range!"));
            }
        }
    }

    for (TActorIterator<AEnemyCharacter> It(GetWorld()); It; ++It)
    {
        It->TakeTurn();
    }
}

void APlayerCharacterController::Click()
{
    if (HoveredCharacter != nullptr)
    {
        ControlledCharacter = HoveredCharacter;

        //Move Camera to character
        CameraPawn->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

        FLatentActionInfo LatentInfo;
        LatentInfo.CallbackTarget = this;
        LatentInfo.ExecutionFunction = FName("CameraAttachToCharacter");
        LatentInfo.Linkage = 0;

        UKismetSystemLibrary::MoveComponentTo(CameraPawn->GetRootComponent(), ControlledCharacter->GetActorLocation(), FRotator::ZeroRotator, false, true,
            CameraData->MoveToCharacterSpeed, false, EMoveComponentAction::Type::Move, LatentInfo);
    }
    else
    {
        MoveCharacter();
    }
}


void APlayerCharacterController::CameraAttachToCharacter()
{
    CameraPawn->AttachToActor(ControlledCharacter,FAttachmentTransformRules::KeepWorldTransform);

    GridManager->ControlledCharacter = ControlledCharacter;
    ControlledCharacter->OnChosen();
    GridManager->UpdateGridPosition();

    UpdatePostProcessMaterial();

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

void APlayerCharacterController::UpdatePostProcessMaterial()
{
    if (!IsValid(PostProcessVolumeRef))
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid PostProcessVolume"));
        return;
    }

    if (!IsValid(PostProcessBlack) || !IsValid(PostProcessWhite))
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid Materials."));
        return;
    }

    // Switch between both material
    if (bUsingMaterialWhite)
    {
        PostProcessVolumeRef->Settings.RemoveBlendable(PostProcessWhite);
        PostProcessVolumeRef->Settings.AddBlendable(PostProcessBlack, 0.1f); 

        GridManager->ValidCellColorIsWhite(false);

        bUsingMaterialWhite = false;
    }
    else
    {
        PostProcessVolumeRef->Settings.RemoveBlendable(PostProcessBlack);
        PostProcessVolumeRef->Settings.AddBlendable(PostProcessWhite, 0.1f);

        GridManager->ValidCellColorIsWhite(true);

        bUsingMaterialWhite = true;
    }
}
