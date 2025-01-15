// Fill out your copyright notice in the Description page of Project Settings.


#include "FFMovementBehavior.h"

// Sets default values for this component's properties
UFFMovementBehavior::UFFMovementBehavior()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...

	Owner = GetOwner();
	if (Owner == nullptr)
	{
		//crash
		//RootComponent = Cast<UPrimitiveComponent>(Owner->GetRootComponent());
	}
}


// Called when the game starts
void UFFMovementBehavior::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UFFMovementBehavior::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UFFMovementBehavior::MoveInDirection(const FVector& Direction, float Speed)
{
	if (RootComponent && RootComponent->IsSimulatingPhysics()) return;

	FVector LinearVelocity = Direction.GetSafeNormal() * Speed;
	RootComponent->SetPhysicsLinearVelocity(LinearVelocity);
}

