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

UENUM()
enum class EInUseStatusOutputPin : uint8
{
	NotInUse,
	InUse
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

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void GetMovement(UPrimitiveComponent* MovableObject, USceneComponent* ToUseTransform);

	UFUNCTION(BlueprintCallable, Category = "Movement", meta = (ExpandEnumAsExecs = "OutputPins"))
	void MoveInDirection(const FVector2D Direction, const float Acceleration, const float Deceleration, const float MaxSpeed, EInUseStatusOutputPin& OutputPins);
	
	UFUNCTION(BlueprintCallable, Category = "Movement", meta = (ExpandEnumAsExecs = "OutputPins"))
	void GroundCheckGravity(const float Gravity, const UCurveFloat* Curve, const float Timer, const float MaxTime, FHitResult& GroundHit, float TraceSize, EGroundStatusOutputPin& OutputPins);

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void JumpBehavior(const float JumpVelo, const UCurveFloat* Curve, const float MaxTime, const float Timer);

	UFUNCTION(BlueprintCallable, Category = "Movement", meta = (ExpandEnumAsExecs = "OutputPins"))
	void Slide(const bool IsSlide, const float SlideMultiply, const float MaxSlideSpeed, const UCurveFloat* Curve, float MaxTime, FVector SlopeNormal, EInUseStatusOutputPin& OutputPins);

	UFUNCTION(BlueprintCallable, Category = "Movement")
	FVector GiveVelocity(bool Grounded, const FVector GroundNormal, const FVector Offset, const float Dist, bool& hitRoof);

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void AddExternalForce(FVector Force);
};