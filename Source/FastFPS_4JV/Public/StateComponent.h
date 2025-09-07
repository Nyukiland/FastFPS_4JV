#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameController.h"
#include "StateComponent.generated.h"


UCLASS(Abstract, Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FASTFPS_4JV_API UStateComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	AGameController* Owner;

public:
	bool Enable;

public:	
	UStateComponent();
	// Called every frame
	virtual void InitStateComponent(AGameController* Controller);
	virtual void EnableStateComponent();
	virtual void DisableStateComponent();
	virtual void TickStateComponent(float DeltaTime);
};