#include "StateComponent.h"

// Sets default values for this component's properties
UStateComponent::UStateComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

void UStateComponent::InitStateComponent_Implementation(AGameController* Controller)
{
	Owner = Controller;
}

void UStateComponent::EnableStateComponent_Implementation()
{

}

void UStateComponent::DisableStateComponent_Implementation()
{

}

void UStateComponent::TickStateComponent_Implementation(float DeltaTime)
{

}