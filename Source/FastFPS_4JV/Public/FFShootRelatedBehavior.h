// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FFShootRelatedBehavior.generated.h"


UENUM()
enum class EShootStatusOutputPin : uint8
{
	NoHit,
	Hit
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class FASTFPS_4JV_API UFFShootRelatedBehavior : public UActorComponent
{
	GENERATED_BODY()

public:
	UFFShootRelatedBehavior();

public:
	UFUNCTION(BlueprintCallable, Category = "Aim")
	void LookAround(USceneComponent* Pivot, USceneComponent* PivotY, const float Speed, const FVector Direction, const float MinClamp, const float MaxClamp);

	UFUNCTION(BlueprintCallable, Category = "Shoot", meta = (ExpandEnumAsExecs = "OutputPins"))
	void ShootTrace(USceneComponent* ShootPoint, AActor* ToIgnore, float Radius, float DistMax, ECollisionChannel TraceChannel, FHitResult& HitResult, EShootStatusOutputPin& OutputPins);

	UFUNCTION(BlueprintCallable, Category = "Shoot", meta = (ExpandEnumAsExecs = "OutputPins"))
	void ShootLaserTrace(USceneComponent* ShootPoint, AActor* ToIgnore, float Radius, float DistMax, ECollisionChannel TraceChannel, TArray<FHitResult>& HitResults, float& ImpactDist, EShootStatusOutputPin& OutputPins);
};
