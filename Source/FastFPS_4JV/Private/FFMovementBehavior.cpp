// Fill out your copyright notice in the Description page of Project Settings.


#include "FFMovementBehavior.h"

// Sets default values for this component's properties
UFFMovementBehavior::UFFMovementBehavior()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

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

void UFFMovementBehavior::MoveInDirection(const FVector Direction, const float Speed, UPrimitiveComponent* MovableObject, USceneComponent* ToUseTransform, bool UseObjectForwardWithHeight)
{
	if (!MovableObject || !MovableObject->IsSimulatingPhysics())
	{
		UE_LOG(LogTemp,
			Error,
			TEXT("UFFMovementBehavior: \n MovableObject Validity: %s \n Simulating physics Validity: %s"),
			MovableObject ? TEXT("true") : TEXT("false"),
			(MovableObject && MovableObject->IsSimulatingPhysics()) ? TEXT("true") : TEXT("false")
		);
		return;
	}

	FVector LinearVelocity = FVector(0, 0, 0);

	if (!ToUseTransform)
	{
		LinearVelocity = Direction.GetSafeNormal() * Speed;
		LinearVelocity.Z = Direction.Z == 0 ? MovableObject->GetPhysicsLinearVelocity().Z : Direction.Z; //Gravity
	}
	else
	{
		LinearVelocity = ToUseTransform->GetForwardVector() * Direction.X;
		LinearVelocity += ToUseTransform->GetRightVector() * Direction.Y;

		if (!UseObjectForwardWithHeight) LinearVelocity.Z = 0;

		LinearVelocity = LinearVelocity.GetSafeNormal() * Speed;
	}

	MovableObject->SetPhysicsLinearVelocity(LinearVelocity);
}

