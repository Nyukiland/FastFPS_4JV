#include "CapacityComponent.h"
#include "CapacityTrigger.h"
#include "CapacityEffect.h"

bool UCapacityComponent::CheckValidity()
{
	if (Triggers.Num() == 0 || Effects.Num() == 0)
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

	for (UCapacityEffect* Effect : Effects)
	{
		if (!Effect) continue;
		Effect->ApplyEffect(Source, nullptr);
	}
}

bool UCapacityComponent::CheckTriggers()
{
	int TriggerCount = 0;
	int TriggerValid = 0;
	
	for (UCapacityTrigger* Trigger : Triggers)
	{
		if (!Trigger) continue;

		TriggerCount++;
		if (Trigger->TriggerReady) TriggerValid++;
	}

	return TriggerValid == TriggerCount;
}

void UCapacityComponent::EnableStateComponent_Implementation()
{
	
}

void UCapacityComponent::DisableStateComponent_Implementation()
{

}

void UCapacityComponent::TickStateComponent_Implementation(float DeltaTime)
{
	if (!CheckValidity())
		return;

	for (UCapacityTrigger* Trigger : Triggers)
	{
		if (!Trigger) continue;
		Trigger->TickTrigger(DeltaTime);
	}

	if (CheckTriggers())
	{
		ExecuteEffect();
		for (UCapacityTrigger* Trigger : Triggers)
		{
			if (!Trigger) continue;
			Trigger->ResetTrigger();
		}
	}
}
