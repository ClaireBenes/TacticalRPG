#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PlayerCharacterController.generated.h"

class UInputAction;
class UInputMappingContext;
class UCameraData;
class AGridManager;
class APlayerCharacter;
class ACameraPawn;

UCLASS()
class TACTICALRPG_API APlayerCharacterController : public APlayerController
{
	GENERATED_BODY()	

public:
    APlayerCharacterController();

protected:
    virtual void BeginPlay() override;
    virtual void SetupInputComponent() override;

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data Asset")
    UCameraData* CameraData = nullptr;

private:
    // **** Functions ****
    UFUNCTION(BlueprintCallable, Category = "Movement")
    void MoveCharacter(); // Handles character movement

    //Input
    UFUNCTION(BlueprintCallable, Category = "Input")
    void Click(); // Handles mouse click

    UFUNCTION(BlueprintCallable, Category = "Input")
    void Zoom(const FInputActionValue& Value); // Handles mouse zoom

    UFUNCTION(BlueprintCallable, Category = "Input")
    void MoveCamera(const FInputActionValue& Value); // Handles camera movement

    UFUNCTION(BlueprintCallable, Category = "Input")
    void RotateCamera(); // Handles camera rotation

private:
    // **** Properties ****
    UPROPERTY(EditAnywhere, Category = "Input")
    UInputAction* ClickInputAction = nullptr;

    UPROPERTY(EditAnywhere, Category = "Input")
    UInputAction* ZoomInputAction = nullptr;

    UPROPERTY(EditAnywhere, Category = "Input")
    UInputAction* MoveCameraInputAction = nullptr;

    UPROPERTY(EditAnywhere, Category = "Input")
    UInputAction* GyroscopeCameraInputAction = nullptr;

    UPROPERTY(EditAnywhere, Category = "Input")
    UInputMappingContext* DefaultInputMappingContext = nullptr;

    APlayerCharacter* ControlledCharacter = nullptr;
    AGridManager* GridManager = nullptr;
    ACameraPawn* CameraPawn = nullptr;
};
