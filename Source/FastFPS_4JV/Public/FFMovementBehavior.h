// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FFMovementBehavior.generated.h"

UENUM()
enum class EGroundStatusOutputPin : uint8
{
	Grounded,
	NotGrounded
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FASTFPS_4JV_API UFFMovementBehavior : public UActorComponent
{
	GENERATED_BODY()

private:
	UPrimitiveComponent* ObjectToMove;
	USceneComponent* ObjectTransformMovement;
	TArray<FVector> AwaitingForce;
	float SlideTimer;
	bool JumpDoOnce;
	FVector SlideDir;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FVector CurVelocity;

protected:
	virtual void BeginPlay() override;

private:
	bool IsMovementReady();


public:	
	UFFMovementBehavior();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void GetMovement(UPrimitiveComponent* MovableObject, USceneComponent* ToUseTransform);

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void MoveInDirection(const FVector2D Direction, const float Acceleration, const float Deceleration, const float MaxSpeed);

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void MoveInAir(const FVector2D Direction, const float Acceleration, const float Deceleration, const float MaxSpeed);
	
	UFUNCTION(BlueprintCallable, Category = "Movement", meta = (ExpandEnumAsExecs = "OutputPins"))
	void IsGrounded(FHitResult& GroundHit, float TraceSize, EGroundStatusOutputPin& OutputPins);

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void JumpBehavior(const float JumpForce, const UCurveFloat* Curve, const float MaxTime, const float Timer);

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void Slide(const bool IsSlide, const float SlideMultiply, const UCurveFloat* Curve, float MaxTime, FVector SlopeNormal);

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void GiveVelocity();

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void Gravity(const float Gravity);

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void AddExternalForce(FVector Force);
};
