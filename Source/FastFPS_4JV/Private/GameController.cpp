#include "GameController.h"
#include "StateComponent.h"
#include "Components/ActorComponent.h"

AGameController::AGameController()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AGameController::BeginPlay()
{
	Super::BeginPlay();
	
	TArray<UActorComponent*> Components;
	GetComponents(Components);

	for (UActorComponent* Comp : Components)
	{
		if (UStateComponent* StateComp = Cast<UStateComponent>(Comp))
		{
			StateComponents.Add(StateComp);
		}
	}
}

void AGameController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGameController::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

UStateComponent* AGameController::GetStateComponentByClass(TSubclassOf<UStateComponent> ComponentClass) const
{
	for (UStateComponent* Comp : StateComponents)
	{
		if (Comp && Comp->IsA(ComponentClass))
		{
			return Comp;
		}
	}
	return nullptr;
}