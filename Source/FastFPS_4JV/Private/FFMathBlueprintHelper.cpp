// Fill out your copyright notice in the Description page of Project Settings.


#include "FFMathBlueprintHelper.h"

float& UFFMathBlueprintHelper::IncrementFloat(UPARAM(ref) float& Value, float Amount)
{
	Value += Amount;
	return Value;
}

int& UFFMathBlueprintHelper::IncrementInt(UPARAM(ref) int& Value, int Amount)
{
	Value += Amount;
	return Value;
}

float& UFFMathBlueprintHelper::DecrementFloat(UPARAM(ref) float& Value, float Amount)
{
	Value -= Amount;
	return Value;
}

int& UFFMathBlueprintHelper::DecrementInt(UPARAM(ref) int& Value, int Amount)
{
	Value -= Amount;
	return Value;
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

void UFFMathBlueprintHelper::RandomOutput(ERandomPinOut& OutputPins)
{
	bool rand = FMath::RandBool();
	if (rand) OutputPins = ERandomPinOut::Random1;
	else OutputPins = ERandomPinOut::Random2;
}

float& UFFMathBlueprintHelper::ClampFloatByRef(UPARAM(ref) float& Value, const float Min, const float Max)
{
	if (Value < Min) Value = Min;
	else if (Value > Max) Value = Max;

	return Value;
}

int& UFFMathBlueprintHelper::ClampIntByRef(UPARAM(ref) int& Value, const int Min, const int Max)
{
	if (Value < Min) Value = Min;
	else if (Value > Max) Value = Max;

	return Value;
}

FVector UFFMathBlueprintHelper::GetOffsetBasedOnTransform(const FVector Offset, const USceneComponent* ToUseTransform)
{
	if (ToUseTransform == nullptr) return FVector::ZeroVector;
	FVector Temp = FVector(0, 0, 0);

	Temp = ToUseTransform->GetForwardVector() * Offset.X;
	Temp += ToUseTransform->GetRightVector() * Offset.Y;
	Temp += ToUseTransform->GetUpVector() * Offset.Z;

	Temp += ToUseTransform->GetComponentLocation();
	Temp = FVector(FMath::RoundToInt(Temp.X), FMath::RoundToInt(Temp.Y), FMath::RoundToInt(Temp.Z));

	return Temp;
}

FVector UFFMathBlueprintHelper::RandomVector(const float Intensity)
{
	float T = FMath::RandRange(0.0f, Intensity);
	FVector Random = FVector(T, T, T);

    return Random.GetClampedToMaxSize(Intensity);
}