// Fill out your copyright notice in the Description page of Project Settings.


#include "FFShootRelatedBehavior.h"

// Sets default values for this component's properties
UFFShootRelatedBehavior::UFFShootRelatedBehavior()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UFFShootRelatedBehavior::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UFFShootRelatedBehavior::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UFFShootRelatedBehavior::LookAround(AActor* ActorToRotate, const float Speed, const FVector Direction, const float MinClamp, const float MaxClamp)
{
	if (!ActorToRotate)
	{
		UE_LOG(LogTemp, Error, TEXT("No Actor to rotate"))
		return;
	}

	FRotator CurrentRotation = ActorToRotate->GetActorRotation();

	FRotator DeltaRotation = Direction.Rotation() * Speed * GetWorld()->GetDeltaSeconds();

	FRotator NewRotation = CurrentRotation + DeltaRotation;

	NewRotation.Yaw = FMath::Clamp(NewRotation.Yaw, MinClamp, MaxClamp);

	ActorToRotate->SetActorRotation(NewRotation);
}

