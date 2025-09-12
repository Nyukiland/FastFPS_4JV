#include "State.h"
#include "StateComponent.h"
#include "GameController.h"

void UState::OnEnter_Implementation(AGameController* GameController)
{
	Owner = GameController;
}

void UState::OnExit_Implementation()
{

}

void UState::Tick_Implementation(float DeltaTime)
{

}

UStateComponent* UState::GetStateComponentByClass(TSubclassOf<UStateComponent> ComponentClass) const
{
	if (!Owner)
	{
		return nullptr;
	}

	UStateComponent* StateComponent = Owner.GetStateComponentByClass<UStateComponent>();

	return ;
}
