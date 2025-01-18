// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FFShootRelatedBehavior.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class FASTFPS_4JV_API UFFShootRelatedBehavior : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UFFShootRelatedBehavior();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Aim")
	void LookAround(USceneComponent* Pivot, USceneComponent* PivotY, const float Speed, const FVector Direction, const float MinClamp, const float MaxClamp);

	UFUNCTION(BlueprintCallable, Category = "Shoot")
	void ShootLineTrace(USceneComponent* ShootPoint, FHitResult& HitResult);

	UFUNCTION(BlueprintCallable, Category = "Shoot")
	void ShootSphereTrace(USceneComponent* ShootPoint, float Radius, TArray<FHitResult>& HitResults);
};
