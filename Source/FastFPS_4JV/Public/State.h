#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "State.generated.h"

class AGameController;
class UStateComponent;

UCLASS(Abstract, Blueprintable)
class FASTFPS_4JV_API UState : public UObject
{
	GENERATED_BODY()

protected:
	UPROPERTY()
	AGameController* Owner;

public:
	UFUNCTION(BlueprintNativeEvent, Category = "State")
	void OnEnter(AGameController* GameController);
	virtual void OnEnter_Implementation(AGameController* GameController);
	
	UFUNCTION(BlueprintNativeEvent, Category = "State")
	void OnExit();
	virtual void OnExit_Implementation();

	UFUNCTION(BlueprintNativeEvent, Category = "State")
	void OnTick(float DeltaTime);
	virtual void OnTick_Implementation(float DeltaTime);
};
