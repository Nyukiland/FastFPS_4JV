// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "FFMathBlueprintHelper.generated.h"

UENUM()
enum class ERandomPinOut : uint8
{
	Random1,
	Random2
};

UCLASS()
class FASTFPS_4JV_API UFFMathBlueprintHelper : public UObject
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "Math Helper")
	static float IncrementFloat(UPARAM(ref) float& Value, float Amount);

	UFUNCTION(BlueprintCallable, Category = "Math Helper")
	static int IncrementInt(UPARAM(ref) int& Value, int Amount);

	UFUNCTION(BlueprintCallable, Category = "Math Helper")
	static float DecrementFloat(UPARAM(ref) float& Value, float Amount);

	UFUNCTION(BlueprintCallable, Category = "Math Helper")
	static int DecrementInt(UPARAM(ref) int& Value, int Amount);

	UFUNCTION(BlueprintCallable, Category = "Math Helper|Vector")
	static float GetAngleBetweenVectors(const FVector VectorA = FVector(0,0,0), const FVector VectorB = FVector(0,0,0));

	UFUNCTION(BlueprintCallable, Category = "Math Helper|Vector")
	static FVector GetClosestPoint(const FVector FirstVector, const TArray<FVector> Vectors);

	UFUNCTION(BlueprintCallable, Category = "Math Helper|Vector")
	static AActor* GetClosestActor(const AActor* BaseActor, const TArray<AActor*> Actors);

	UFUNCTION(BlueprintCallable, Category = "Math Helper|Random")
	static FVector2D RandomPointInCircle(float Radius = 0);

	UFUNCTION(BlueprintCallable, Category = "Math Helper|Random")
	static FVector RandomPointInSphere(float Radius = 0);

	UFUNCTION(BlueprintCallable, Category = "Math Helper", meta = (ExpandEnumAsExecs = "OutputPins"))
	static void RandomOutput(ERandomPinOut& OutputPins);

	UFUNCTION(BlueprintCallable, Category = "Math Helper")
	static void ClampFloatByRef(UPARAM(ref) float& Value, const float Min, const float Max);

	UFUNCTION(BlueprintCallable, Category = "Math Helper")
	static void ClampIntByRef(UPARAM(ref) int& Value, const int Min, const int Max);
};