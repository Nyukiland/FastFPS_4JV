#include "CapacityComponent.h"
#include "CapacityTrigger.h"
#include "CapacityEffect.h"

bool UCapacityComponent::CheckValidity()
{
	if (!Trigger || !Effect)
	{
		UE_LOG(LogTemp, Error, TEXT("Wrong set up on %s"), *this->GetName());
		return false;
	}

	return true;
}

void UCapacityComponent::ExecuteEffect()
{
	if (!CheckValidity())
		return;

	AActor* Source = GetTypedOuter<AActor>();
	Effect->ApplyEffect(Source, nullptr);
}

void UCapacityComponent::EnableStateComponent_Implementation()
{
	if (!CheckValidity())
		return;

	Trigger->OnTriggered.AddDynamic(this, &UCapacityComponent::ExecuteEffect);
}

void UCapacityComponent::DisableStateComponent_Implementation()
{
	if (!CheckValidity())
		return;

	Trigger->OnTriggered.RemoveDynamic(this, &UCapacityComponent::ExecuteEffect);
}

void UCapacityComponent::TickStateComponent_Implementation(float DeltaTime)
{
	if (!CheckValidity())
		return;

	Trigger->TickTrigger(DeltaTime);
}