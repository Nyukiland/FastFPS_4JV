// Fill out your copyright notice in the Description page of Project Settings.


#include "FFMathBlueprintHelper.h"

void UFFMathBlueprintHelper::IncrementFloat(float& Value, float Amount)
{
	Value += Amount;
}

void UFFMathBlueprintHelper::IncrementInt(int& Value, int Amount)
{
	Value += Amount;
}

void UFFMathBlueprintHelper::DecrementFloat(float& Value, float Amount)
{
	Value -= Amount;
}

void UFFMathBlueprintHelper::DecrementInt(int& Value, int Amount)
{
	Value -= Amount;
}

float UFFMathBlueprintHelper::GetAngleBetweenVectors(const FVector VectorA, const FVector VectorB)
{
	float DotProduct = FVector::DotProduct(VectorA.GetSafeNormal(), VectorB.GetSafeNormal());
	return FMath::Acos(DotProduct) * (180.0f / PI);
}

FVector UFFMathBlueprintHelper::GetClosestPoint(const FVector FirstVector, const TArray<FVector> Vectors)
{
	if (Vectors.Num() <= 0)
	{
		UE_LOG(LogTemp, Error, TEXT("[UFFMathBlueprintHelper::GetClosestPoint] List Is Empty"));
		return FVector(0, 0, 0);
	}

	FVector Closest = Vectors[0];
	float DistClosest = FVector::Dist(FirstVector, Vectors[0]);

	for(const FVector Vector : Vectors)
	{
		float CurDist = FVector::Dist(FirstVector, Vector);
		if (CurDist < DistClosest)
		{
			Closest = Vector;
			DistClosest = CurDist;
		}
	}

	return Closest;
}

AActor* UFFMathBlueprintHelper::GetClosestActor(const AActor* BaseActor, const TArray<AActor*> Actors)
{
	if (!BaseActor || Actors.Num() <= 0)
	{
		UE_LOG(LogTemp, Error, TEXT("[UFFMathBlueprintHelper::GetClosestPoint] List Is Empty or Self is not set"));
		return nullptr;
	}

	AActor* Closest = Actors[0];
	float DistClosest = FVector::Dist(BaseActor->GetActorLocation(), Actors[0]->GetActorLocation());

	for (AActor* Actor : Actors)
	{
		float CurDist = FVector::Dist(BaseActor->GetActorLocation(), Actor->GetActorLocation());
		if (CurDist < DistClosest)
		{
			Closest = Actor;
			DistClosest = CurDist;
		}
	}

	return Closest;
}

FVector2D UFFMathBlueprintHelper::RandomPointInCircle(float Radius)
{
	float RandomX = FMath::RandRange(0, 1);
	float RandomY = FMath::RandRange(0, 1);
	return FVector2D(RandomX, RandomY).GetSafeNormal() * Radius;
}

FVector UFFMathBlueprintHelper::RandomPointInSphere(float Radius)
{
	float RandomX = FMath::RandRange(0, 1);
	float RandomY = FMath::RandRange(0, 1);
	float RandomZ = FMath::RandRange(0, 1);
	return FVector(RandomX, RandomY, RandomZ).GetSafeNormal() * Radius;
}
