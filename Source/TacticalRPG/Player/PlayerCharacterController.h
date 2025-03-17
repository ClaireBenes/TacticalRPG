#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PlayerCharacterController.generated.h"

class UInputAction;
class UInputMappingContext;
class AGridManager;
class APlayerCharacter;

UCLASS()
class TACTICALRPG_API APlayerCharacterController : public APlayerController
{
	GENERATED_BODY()	

public:
    APlayerCharacterController();

protected:
    virtual void BeginPlay() override;
    virtual void SetupInputComponent() override;

private:
    UFUNCTION(BlueprintCallable, Category = "Movement")
    void OnClickMove(); // Handles mouse click movement

    UPROPERTY(EditAnywhere, Category = "Input")
    UInputAction* ClickInputAction = nullptr;

    UPROPERTY(EditAnywhere, Category = "Input")
    UInputMappingContext* DefaultInputMappingContext = nullptr;

    APlayerCharacter* ControlledCharacter;
    AGridManager* GridManager;
};
