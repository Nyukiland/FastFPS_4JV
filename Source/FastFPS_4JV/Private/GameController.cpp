#include "Components/ActorComponent.h"
#include "GameController.h"
#include "StateComponent.h"
#include "State.h"

AGameController::AGameController()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AGameController::BeginPlay()
{
	Super::BeginPlay();

	ChangeState(DefaultState);

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

	if (CurrentState)
	{
		CurrentState->OnTick(DeltaTime);
	}

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

//Get StateComponent -------------------------------------------------------------------------------------------------
UStateComponent* AGameController::GetStateComponentByClass(TSubclassOf<UStateComponent> ComponentClass)
{
	int I = 0;
	return GetStateComponentByClass(ComponentClass, I);
}

UStateComponent* AGameController::GetStateComponentByClass(TSubclassOf<UStateComponent> ComponentClass, int& Index)
{
	Index = -1;

	for (int i = 0; i < StateComponents.Num(); i++)
	{
		UStateComponent* Comp = StateComponents[i];

		if (Comp && Comp->IsA(ComponentClass))
		{
			Index = i;
			return Comp;
		}
	}

	UE_LOG(LogTemp, Error, TEXT("Failed to get component of class: %s"), ComponentClass->GetName());

	return nullptr;
}

//Activate StateComponent -------------------------------------------------------------------------------------------------
void AGameController::ActivateStateComponent(TSubclassOf<UStateComponent> ComponentClass)
{
	int I = 0;
	UStateComponent* Comp = GetStateComponentByClass(ComponentClass, I);
	ActivateStateComponent(Comp, I);
}

void AGameController::ActivateStateComponent(UStateComponent* Comp, int Index)
{
	if (!Comp || Index < 0 || Index > StateComponents.Num())
		return;

	StateComponents.Swap(Index, ActiveComponentCount);
	Comp->EnableStateComponent();
	ActiveComponentCount++;
}

//Deactivate StateComponent -------------------------------------------------------------------------------------------------
void AGameController::DeactivateStateComponent(TSubclassOf<UStateComponent> ComponentClass)
{
	int I = 0;
	UStateComponent* Comp = GetStateComponentByClass(ComponentClass, I);
	DeactivateStateComponent(Comp, I);
}

void AGameController::DeactivateStateComponent(UStateComponent* Comp, int Index)
{
	if (!Comp || Index < 0 || Index > StateComponents.Num())
		return;

	StateComponents.Swap(Index, ActiveComponentCount - 1);
	Comp->DisableStateComponent();
	ActiveComponentCount--;
}

//Change State -------------------------------------------------------------------------------------------------
void AGameController::ChangeState(TSubclassOf<UState> StateClass)
{
	if (!*StateClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("ChangeState called with null class."));
		return;
	}

	if (CurrentState)
	{
		CurrentState->OnExit();
		CurrentState = nullptr;
	}

	UState* NewState = NewObject<UState>(this, StateClass);
	if (!NewState)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create state of class: %s"), *StateClass->GetName());
		return;
	}

	NewState->OnEnter(this);
	CurrentState = NewState;
}