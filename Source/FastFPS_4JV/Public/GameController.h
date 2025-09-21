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
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "State")
	TSubclassOf<UState> DefaultState;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "State")
	TArray<TSubclassOf<UStateComponent>> DefaultActiveComponent;

private:
	TArray<UStateComponent*> StateComponents;
	int ActiveComponentCount = 0;
	UState* CurrentState;

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable, Category = "State")
	UStateComponent* GetStateComponentByClass(TSubclassOf<UStateComponent> ComponentClass);
	UStateComponent* GetStateComponentByClass(TSubclassOf<UStateComponent> ComponentClass, int& Index);

	UFUNCTION(BlueprintCallable, Category = "State")
	void ActivateStateComponent(TSubclassOf<UStateComponent> ComponentClass);

	void ActivateStateComponent(UStateComponent* Comp, int Index);

	UFUNCTION(BlueprintCallable, Category = "State")
	void DeactivateStateComponent(TSubclassOf<UStateComponent> ComponentClass);

	void DeactivateStateComponent(UStateComponent* Comp, int Index);

	UFUNCTION(BlueprintCallable, Category = "State")
	void ChangeState(TSubclassOf<UState> StateClass);
};
