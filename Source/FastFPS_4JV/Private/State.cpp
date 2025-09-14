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

void UState::OnTick_Implementation(float DeltaTime)
{

}