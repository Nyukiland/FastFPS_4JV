// Fill out your copyright notice in the Description page of Project Settings.


#include "FFShootRelatedBehavior.h"
#include "FFEnemyManager.h"
#include "Kismet/KismetSystemLibrary.h"

UFFShootRelatedBehavior::UFFShootRelatedBehavior()
{
	PrimaryComponentTick.bCanEverTick = true;

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

void UFFShootRelatedBehavior::ShootTrace(USceneComponent* ShootPoint, AActor* ToIgnore, float Radius, float DistMax, ECollisionChannel TraceChannel, FHitResult& HitResult, EShootStatusOutputPin& OutputPins)
{
	if (!ShootPoint)
	{
		UE_LOG(LogTemp, Error, TEXT("No Shooting Point!"));
		return;
	}

	FVector Start = ShootPoint->GetComponentLocation();
	FVector ForwardVector = ShootPoint->GetForwardVector();
	FVector End = Start + (ForwardVector * DistMax);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(ToIgnore);
	QueryParams.bReturnPhysicalMaterial = true;

	bool bHit = GetWorld()->SweepSingleByChannel(HitResult, Start, End, FQuat::Identity, TraceChannel, FCollisionShape::MakeSphere(Radius), QueryParams);

	OutputPins = bHit ? EShootStatusOutputPin::Hit : EShootStatusOutputPin::NoHit;
}


void UFFShootRelatedBehavior::ShootLaserTrace(USceneComponent* ShootPoint, AActor* ToIgnore, float Radius, float DistMax, ECollisionChannel TraceChannel, TArray<FHitResult>& HitResults, float& ImpactDist, EShootStatusOutputPin& OutputPins)
{
	if (!ShootPoint)
	{
		UE_LOG(LogTemp, Error, TEXT("No Shooting Point!"));
		return;
	}

	FVector Start = ShootPoint->GetComponentLocation();
	FVector ForwardVector = ShootPoint->GetForwardVector();
	FVector End = Start + (ForwardVector * DistMax);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(ToIgnore);
	TArray<AActor*> EnemyActors = UFFEnemyManager::GetEnemyManager(GetWorld())->GetAllEnemies();
	QueryParams.AddIgnoredActors(EnemyActors);
	QueryParams.bReturnPhysicalMaterial = true;

	FHitResult LineHit;
	bool bLineHit = GetWorld()->LineTraceSingleByChannel(LineHit, Start, End, TraceChannel, QueryParams);

	if (bLineHit)
	{
		End = LineHit.ImpactPoint;
	}

	TArray<AActor*> Ignore;
	Ignore.Add(ToIgnore);
	ImpactDist = FVector::Dist(Start, End);

	UKismetSystemLibrary::SphereTraceMulti(GetWorld(), Start, End, Radius, UEngineTypes::ConvertToTraceType(TraceChannel), false, Ignore, EDrawDebugTrace::None, HitResults, true);

	OutputPins = HitResults.Num() > 0 ? EShootStatusOutputPin::Hit : EShootStatusOutputPin::NoHit;

	DrawDebugLine(GetWorld(), Start, End, bLineHit ? FColor::Green : FColor::Red, false, 2.0f, 0, 1.0f);
	DrawDebugSphere(GetWorld(), End, Radius, 12, FColor::Blue, false, 2.0f);

	UE_LOG(LogTemp, Error, TEXT("Hit Count: %i"), HitResults.Num());

	for (const FHitResult& Hit : HitResults)
	{
		if (Hit.GetActor())
		{
			UE_LOG(LogTemp, Log, TEXT("Hit Actor: %s"), *Hit.GetActor()->GetName());
		}
	}
}