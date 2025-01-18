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
