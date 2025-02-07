// Fill out your copyright notice in the Description page of Project Settings.


#include "FFMovementBehavior.h"

// Sets default values for this component's properties
UFFMovementBehavior::UFFMovementBehavior()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

}


// Called when the game starts
void UFFMovementBehavior::BeginPlay()
{
	Super::BeginPlay();

	CurVelocity = FVector(0, 0, 0);
}

bool UFFMovementBehavior::IsMovementReady()
{
	if (!ObjectToMove || !ObjectToMove->IsSimulatingPhysics() || !ObjectTransformMovement)
	{
		UE_LOG(LogTemp, Error, TEXT("[UFFMovementBehavior::MoveInDirection] Set up issue"));
		return false;
	}

	return true;
}

void UFFMovementBehavior::GetMovement(UPrimitiveComponent* MovableObject, USceneComponent* ToUseTransform)
{
	if (!MovableObject || !MovableObject->IsSimulatingPhysics() || !ToUseTransform)
	{
		UE_LOG(LogTemp,
			Error,
			TEXT("[UFFMovementBehavior::GetMovement] UFFMovementBehavior: \n MovableObject Validity: %s \n Simulating physics Validity: %s \n ToUseTransfrom: %s"),
			MovableObject ? TEXT("true") : TEXT("false"),
			(MovableObject && MovableObject->IsSimulatingPhysics()) ? TEXT("true") : TEXT("false"),
			ToUseTransform ? TEXT("true") : TEXT("false")
		);
		return;
	}

	ObjectToMove = MovableObject;
	ObjectTransformMovement = ToUseTransform;
}

void UFFMovementBehavior::MoveInDirection(const FVector2D Direction, const float Acceleration, const float Deceleration, const float MaxSpeed, EInUseStatusOutputPin& OutputPins)
{
	if (!IsMovementReady()) return;

	float SpeedToGo = Direction.Length() * MaxSpeed;
	float CurSpeed = FVector(CurVelocity.X, CurVelocity.Y, 0).Length();

	FVector NewVelo = CurVelocity;
	if (Direction != FVector2D(0, 0))
	{
		if (CurSpeed < MaxSpeed) CurSpeed += Acceleration;
		else CurSpeed -= Deceleration;

		NewVelo = ObjectTransformMovement->GetForwardVector() * Direction.X;
		NewVelo += ObjectTransformMovement->GetRightVector() * Direction.Y;

		OutputPins = EInUseStatusOutputPin::InUse;
	}
	else
	{
		CurSpeed -= Deceleration;
		if (CurSpeed < 0) CurSpeed = 0;

		OutputPins = EInUseStatusOutputPin::NotInUse;
	}

	NewVelo.Z = 0;
	NewVelo = NewVelo.GetSafeNormal();
	NewVelo *= CurSpeed;
	CurVelocity = FVector(NewVelo.X, NewVelo.Y, CurVelocity.Z);
}

void UFFMovementBehavior::GroundCheckGravity(const float Gravity, const UCurveFloat* Curve, const float Timer, const float MaxTime, FHitResult& GroundHit, float TraceSize, EGroundStatusOutputPin& OutputPins)
{
	if (!IsMovementReady()) return;
	if (!Curve) return;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(ObjectToMove->GetOwner());

	FVector VectorDown = FVector(0, 0, -TraceSize);

	bool bHit = GetWorld()->LineTraceSingleByChannel(GroundHit, ObjectToMove->GetComponentLocation(), ObjectToMove->GetComponentLocation() + VectorDown, ECC_Visibility, QueryParams);

	if (bHit)
	{
		if (GroundHit.Distance > TraceSize - 5) CurVelocity.Z = -Gravity;
		else CurVelocity.Z = 0;

		OutputPins = EGroundStatusOutputPin::Grounded;
	}
	else
	{
		float Value0To1 = FMath::Clamp(Timer / MaxTime, 0.0f, 1.0f);
		float CurveValue = Curve->GetFloatValue(Value0To1);
		CurVelocity.Z = -Gravity * CurveValue;

		OutputPins = EGroundStatusOutputPin::NotGrounded;
	}
}

void UFFMovementBehavior::JumpBehavior(const float InitialHeight, const float TargetHeight, const UCurveFloat* Curve, const float MaxTime, const float Timer)
{
	if (!IsMovementReady()) return;

	if (!Curve || MaxTime <= 0.0f)
	{
		prevVeloJump = 0;
		return;
	}

	float Value0To1 = FMath::Clamp(Timer / MaxTime, 0.0f, 1.0f);

	float CurveValue = Curve->GetFloatValue(Value0To1);

	float CurrentHeight = ObjectToMove->GetComponentLocation().Z;
	float DesiredHeight = FMath::Lerp(InitialHeight, InitialHeight + TargetHeight, CurveValue);

	float VelocityZ = (DesiredHeight - CurrentHeight) / GetWorld()->DeltaTimeSeconds;

	if (VelocityZ > 0) prevVeloJump = VelocityZ;

	CurVelocity.Z = prevVeloJump;
}

void UFFMovementBehavior::Slide(const bool IsSlide, const float SlideMultiply, const UCurveFloat* Curve, float MaxTime, FVector SlopeNormal, EInUseStatusOutputPin& OutputPins)
{
	if (!IsMovementReady()) return;
	if (!Curve) return;

	float dot;
	dot = FVector::DotProduct(SlopeNormal, FVector(0, 0, 1));
	dot = FMath::Abs(dot);
	OutputPins = EInUseStatusOutputPin::NotInUse;

	if (!IsSlide)
	{
		SlideDir = CurVelocity;
		SlideDir.Z = 0;
		SlideTimer = 0;
		return;
	}
	else if (MaxTime < SlideTimer) return;

	OutputPins = EInUseStatusOutputPin::InUse;

	FVector SlopeDir = FVector::CrossProduct(SlopeNormal, FVector(0, 0, 1));
	SlopeDir = FVector::CrossProduct(SlopeNormal, SlopeDir);
	if (dot < 1 && SlopeDir.Z < 0) SlopeDir *= -1;

	FVector ProjectedSlideDir = SlideDir - FVector::DotProduct(SlideDir, SlopeNormal.GetSafeNormal()) * SlopeNormal.GetSafeNormal();

	SlideTimer += GetWorld()->DeltaTimeSeconds;

	float dotDown = FVector::DotProduct(SlopeNormal, SlideDir);
	if (dot != 1 && dotDown > 0) SlideTimer = 0;

	float Value0To1 = FMath::Clamp(SlideTimer / MaxTime, 0, 1);
	float CurveEval = Curve->GetFloatValue(Value0To1);
	CurveEval = FMath::Lerp(CurveEval, 1, SlideMultiply);

	if (dot == 1) ProjectedSlideDir = SlideDir;

	float StoredZ = CurVelocity.Z;

	CurVelocity = ProjectedSlideDir * CurveEval;
	CurVelocity.Z = StoredZ;
}

void UFFMovementBehavior::GiveVelocity(const FVector GroundNormal, const FVector Offset, const float Dist)
{
	if (!IsMovementReady()) return;

	if (AwaitingForce.Num() > 0)
	{
		for (const FVector& Force : AwaitingForce)
		{
			CurVelocity += Force;
		}
		AwaitingForce.Empty();
	}

	FVector VeloToGive = CurVelocity;

	if (!GroundNormal.IsNearlyZero() && CurVelocity.Z <= 0)
	{
		float Magnitude = VeloToGive.Length();
		VeloToGive = FVector::VectorPlaneProject(VeloToGive, GroundNormal);
		VeloToGive = VeloToGive.GetSafeNormal() * Magnitude;
	}

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(ObjectToMove->GetOwner());
	FHitResult HitResultDir;
	FVector Start = ObjectToMove->GetComponentLocation() + Offset;
	FVector EndDir = Start + (VeloToGive.GetSafeNormal() * Dist);

	bool bHitDir = GetWorld()->LineTraceSingleByChannel(HitResultDir, Start, EndDir, ECC_Visibility, QueryParams);

	ObjectToMove->WakeAllRigidBodies();
	if (bHitDir)
	{
		FVector HitNormal = HitResultDir.ImpactNormal;
		float DotProduct = FVector::DotProduct(VeloToGive.GetSafeNormal(), HitNormal);

		if (VeloToGive.Z <= 0)
		{
			float Magnitude = VeloToGive.Length();
			VeloToGive = FVector::VectorPlaneProject(VeloToGive, HitNormal);
			VeloToGive = VeloToGive.GetSafeNormal() * Magnitude;
			ObjectToMove->SetPhysicsLinearVelocity(VeloToGive);
		}

		/*if (FMath::Abs(DotProduct) > 0.65f)
		{
			if (VeloToGive.Z < 0)
			{
				ObjectToMove->SetPhysicsLinearVelocity(FVector(0, 0, VeloToGive.Z));
			}
			else ObjectToMove->SetPhysicsLinearVelocity(FVector::ZeroVector);
		}
		else
		{
			if (VeloToGive.Z <= 0)
			{
				float Magnitude = VeloToGive.Length();
				VeloToGive = FVector::VectorPlaneProject(VeloToGive, HitNormal);
				VeloToGive = VeloToGive.GetSafeNormal() * Magnitude;
				ObjectToMove->SetPhysicsLinearVelocity(VeloToGive);
			}
		}*/
	}
	else
	{
		ObjectToMove->SetPhysicsLinearVelocity(VeloToGive);
	}
}

void UFFMovementBehavior::AddExternalForce(FVector Force)
{
	AwaitingForce.Add(Force);
}