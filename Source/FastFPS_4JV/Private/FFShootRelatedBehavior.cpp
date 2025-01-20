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

void UFFShootRelatedBehavior::LookAround(USceneComponent* Pivot, USceneComponent* PivotY, const float Speed, const FVector Direction, const float MinClamp, const float MaxClamp)
{
	if (!Pivot || !PivotY)
	{
		UE_LOG(LogTemp, Error, TEXT("[LookAround] No Actor to rotate"))
			return;
	}

	FRotator CurrentRotation = Pivot->GetRelativeRotation();
	FRotator CurrentRotationY = PivotY->GetRelativeRotation();

	FRotator Rotation = FRotator(Direction.X, Direction.Y, Direction.Z) * Speed;

	FRotator NewRotation = CurrentRotation + FRotator(0.f, Rotation.Yaw, 0.f);

	FRotator NewRotationY = CurrentRotationY + FRotator(Rotation.Pitch, 0.f, 0.f);

	NewRotationY.Pitch = FMath::Clamp(NewRotationY.Pitch, MinClamp, MaxClamp);

	Pivot->SetRelativeRotation(NewRotation);
	PivotY->SetRelativeRotation(NewRotationY);
}

void UFFShootRelatedBehavior::ShootLineTrace(USceneComponent* ShootPoint, float DistMax, FHitResult& HitResult, EShootStatusOutputPin& OutputPins)
{
	if (!ShootPoint)
	{
		UE_LOG(LogTemp, Error, TEXT("No Shooting Point"));
		return;
	}

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(GetOwner());
	QueryParams.bReturnPhysicalMaterial = true;

	float Dist = DistMax < 0 ? 10000 : DistMax;

	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, ShootPoint->GetComponentLocation(), ShootPoint->GetComponentLocation() + (ShootPoint->GetForwardVector() * Dist), ECC_Visibility, QueryParams);

	OutputPins = bHit? EShootStatusOutputPin::Hit : EShootStatusOutputPin::NoHit;
}

void UFFShootRelatedBehavior::ShootSphereTrace(USceneComponent* ShootPoint, float Radius, float DistMax, TArray<FHitResult>& HitResults, EShootStatusOutputPin& OutputPins)
{
	if (!ShootPoint)
	{
		UE_LOG(LogTemp, Error, TEXT("No Shooting Point"));
		return;
	}

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(GetOwner());
	QueryParams.bReturnPhysicalMaterial = true;

	float Dist = DistMax < 0 ? 10000 : DistMax;

	FHitResult firstHit;
	bool LineHit = GetWorld()->LineTraceSingleByChannel(firstHit, ShootPoint->GetComponentLocation(), ShootPoint->GetComponentLocation() + (ShootPoint->GetForwardVector() * Dist), ECC_Visibility, QueryParams);

	if (LineHit) Dist = firstHit.Distance + Radius;

	bool bHit = GetWorld()->SweepMultiByChannel(HitResults, ShootPoint->GetComponentLocation(), ShootPoint->GetComponentLocation() + (ShootPoint->GetForwardVector() * Dist), FQuat::Identity, ECC_Visibility, FCollisionShape::MakeSphere(Radius), QueryParams);
	
	OutputPins = bHit ? EShootStatusOutputPin::Hit : EShootStatusOutputPin::NoHit;
}

