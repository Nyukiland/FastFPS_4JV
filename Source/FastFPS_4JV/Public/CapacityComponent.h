#pragma once

#include "CoreMinimal.h"
#include "StateComponent.h"
#include "CapacityComponent.generated.h"

class UCapacityTrigger;
class UCapacityEffect;

UCLASS()
class FASTFPS_4JV_API UCapacityComponent : public UStateComponent
{
	GENERATED_BODY()
	
private:
	bool CheckValidity();

	void ExecuteEffect();

public:
	UPROPERTY(EditAnywhere, Instanced, BlueprintReadOnly, Category = "Capacity")
	UCapacityTrigger* Trigger;

	UPROPERTY(EditAnywhere, Instanced, BlueprintReadOnly, Category = "Capacity")
	UCapacityEffect* Effect;

	virtual void EnableStateComponent_Implementation() override;

	virtual void DisableStateComponent_Implementation() override;

	virtual void TickStateComponent_Implementation(float DeltaTime) override;

};
