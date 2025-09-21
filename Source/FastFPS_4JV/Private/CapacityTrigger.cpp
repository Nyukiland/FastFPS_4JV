// Fill out your copyright notice in the Description page of Project Settings.


#include "CapacityTrigger.h"

void UCapacityTrigger::TickTrigger_Implementation(float DeltaTime)
{

}

void UCapacityTrigger::ResetTrigger_Implementation()
{
	TriggerReady = false;
}

TArray<UStatContainer*> UCapacityTrigger::GetStats_Implementation()
{
	return TArray<UStatContainer*>();
}