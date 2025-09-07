// Fill out your copyright notice in the Description page of Project Settings.


#include "StateComponent.h"

// Sets default values for this component's properties
UStateComponent::UStateComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

void UStateComponent::InitStateComponent(AGameController* Controller)
{
	Owner = Controller;
}

void UStateComponent::EnableStateComponent()
{
	Enable = true;
}

void UStateComponent::DisableStateComponent()
{
	Enable = false;
}

void UStateComponent::TickStateComponent(float DeltaTime)
{

}