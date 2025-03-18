#include "TacticalRPG/Player/PlayerCharacterController.h"
#include "TacticalRPG/Player/PlayerCharacter.h"
#include "TacticalRPG/Grid/GridManager.h"

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
        EnhancedInputComponent->BindAction(ClickInputAction, ETriggerEvent::Started, this, &APlayerCharacterController::OnClickMove);
    }
}

void APlayerCharacterController::OnClickMove()
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
