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

	bool CheckTriggers();

public:
	UPROPERTY(EditAnywhere, Instanced, BlueprintReadOnly, Category = "Capacity")
	TArray<UCapacityTrigger*> Triggers;

	UPROPERTY(EditAnywhere, Instanced, BlueprintReadOnly, Category = "Capacity")
	TArray<UCapacityEffect*> Effects;

	virtual void EnableStateComponent_Implementation() override;

	virtual void DisableStateComponent_Implementation() override;

	virtual void TickStateComponent_Implementation(float DeltaTime) override;
};
