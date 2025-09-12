#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GameController.generated.h"

class UState;
class UStateComponent;

UCLASS()
class FASTFPS_4JV_API AGameController : public APawn
{
	GENERATED_BODY()

public:
	AGameController();

public:
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "State")
	TSubclassOf<UState> DefaultState;

private:
	TArray<UStateComponent*> StateComponents;

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable, Category = "State")
	UStateComponent* GetStateComponentByClass(TSubclassOf<UStateComponent> ComponentClass) const;
};
