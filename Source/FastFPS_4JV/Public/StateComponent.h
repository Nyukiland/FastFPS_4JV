#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StateComponent.generated.h"

class AGameController;

UCLASS(Abstract, Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FASTFPS_4JV_API UStateComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	AGameController* Owner;

public:	
	UStateComponent();

	UFUNCTION(BlueprintNativeEvent, Category = "State")
	void InitStateComponent(AGameController* Controller);
	virtual void InitStateComponent_Implementation(AGameController* Controller);

	UFUNCTION(BlueprintNativeEvent, Category = "State")
	void EnableStateComponent();
	virtual void EnableStateComponent_Implementation();

	UFUNCTION(BlueprintNativeEvent, Category = "State")
	void DisableStateComponent();
	virtual void DisableStateComponent_Implementation();
	
	UFUNCTION(BlueprintNativeEvent, Category = "State")
	void TickStateComponent(float DeltaTime);
	virtual void TickStateComponent_Implementation(float DeltaTime);
};