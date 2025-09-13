#include "GameController.h"
#include "StateComponent.h"
#include "Components/ActorComponent.h"

AGameController::AGameController()
{
	PrimaryActorTick.bCanEverTick = true;

	ActiveComponentCount = 0;
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
			StateComp->InitStateComponent(this);

			for (TSubclassOf<UStateComponent> DefaultComp : DefaultActiveComponent)
			{
				if (StateComp->IsA(DefaultComp))
				{
					StateComponents.Swap(StateComponents.Num() - 1, ActiveComponentCount);
					ActiveComponentCount++;
					break;
				}
			}
		}
	}

	for (int i = 0; i < ActiveComponentCount; i++)
	{
		if (StateComponents[i])
		StateComponents[i]->EnableStateComponent();
	}
}

void AGameController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	for (int i = 0; i < ActiveComponentCount; i++)
	{
		if (StateComponents[i])
			StateComponents[i]->TickStateComponent(DeltaTime);
	}
}

void AGameController::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

UStateComponent* AGameController::GetStateComponentByClass(TSubclassOf<UStateComponent> ComponentClass, bool activate)
{
	for (int i = 0; i < StateComponents.Num(); i++)
	{
		UStateComponent* Comp = StateComponents[i];

		if (Comp && Comp->IsA(ComponentClass))
		{
			if (activate)
			{
				StateComponents.Swap(i, ActiveComponentCount);
				Comp->EnableStateComponent();
				ActiveComponentCount += 1;
			}
			else
			{
				StateComponents.Swap(i, ActiveComponentCount - 1);
				Comp->DisableStateComponent();
				ActiveComponentCount--;
			}

			return Comp;
		}
	}

	UE_LOG(LogTemp, Error, TEXT("Failed to get component of class: %s"), ComponentClass->GetName());

	return nullptr;
}