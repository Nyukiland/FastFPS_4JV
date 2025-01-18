// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "FFMathBlueprintHelper.generated.h"

UCLASS()
class FASTFPS_4JV_API UFFMathBlueprintHelper : public UObject
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "Math Helper")
	static void IncrementFloat(UPARAM(ref) float& Value, float Amount);

	UFUNCTION(BlueprintCallable, Category = "Math Helper")
	static void IncrementInt(UPARAM(ref) int& Value, int Amount);

	UFUNCTION(BlueprintCallable, Category = "Math Helper")
	static void DecrementFloat(UPARAM(ref) float& Value, float Amount);

	UFUNCTION(BlueprintCallable, Category = "Math Helper")
	static void DecrementInt(UPARAM(ref) int& Value, int Amount);

	UFUNCTION(BlueprintCallable, Category = "Math Helper|Vector")
	static float GetAngleBetweenVectors(const FVector VectorA, const FVector VectorB);

	UFUNCTION(BlueprintCallable, Category = "Math Helper|Random")
	static FVector2D RandomPointInCircle(float Radius);

	UFUNCTION(BlueprintCallable, Category = "Math Helper|Random")
	static FVector RandomPointInSphere(float Radius);
};
