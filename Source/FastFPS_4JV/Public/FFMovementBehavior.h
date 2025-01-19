// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FFMovementBehavior.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FASTFPS_4JV_API UFFMovementBehavior : public UActorComponent
{
	GENERATED_BODY()

private:
	UPrimitiveComponent* ObjectToMove;
	USceneComponent* ObjectTransformMovement;
	FVector CurVelocity;

protected:
	virtual void BeginPlay() override;

public:	
	UFFMovementBehavior();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void GetMovement(UPrimitiveComponent* MovableObject, USceneComponent* ToUseTransform);

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void MoveInDirection(const FVector2D Direction, const float Acceleration, const float Deceleration, const float MaxSpeed);

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void GiveVelocity();
};
