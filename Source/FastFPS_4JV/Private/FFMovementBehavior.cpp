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

void UFFMovementBehavior::GroundCheckGravity(const float Gravity, const UCurveFloat* Curve, const float Timer, const float MaxTime, FHitResult& GroundHit, float TraceSize, float TraceTolerance, EGroundStatusOutputPin& OutputPins)
{
	if (!IsMovementReady()) return;
	if (!Curve) return;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(ObjectToMove->GetOwner());

	FVector VectorDown = FVector(0, 0, -TraceSize);

	bool bHit = GetWorld()->LineTraceSingleByChannel(GroundHit, ObjectToMove->GetComponentLocation(), ObjectToMove->GetComponentLocation() + VectorDown, ECC_Visibility, QueryParams);

	DrawDebugLine(GetWorld(), ObjectToMove->GetComponentLocation(), ObjectToMove->GetComponentLocation() + VectorDown, FColor::Red, false, 1.0f, 0, 5.0f);
	DrawDebugLine(GetWorld(), ObjectToMove->GetComponentLocation(), ObjectToMove->GetComponentLocation() + (VectorDown - FVector(0, 0, TraceTolerance)), FColor::Green, false, 1.0f, 0, 6.0f);

	if (bHit)
	{
		if (GroundHit.Distance > TraceSize - TraceTolerance) CurVelocity.Z = -Gravity;
		CurVelocity.Z = 0;

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

	if (!Curve || Timer <= 0.0f)
	{
		prevVeloJump = 0;
		return;
	}

	float Value0To1 = FMath::Clamp(Timer / MaxTime, 0.0f, 1.0f);
	float CurveValue = Curve->GetFloatValue(Value0To1);

	float CurrentHeight = ObjectToMove->GetComponentLocation().Z;
	float DesiredHeight = FMath::Lerp(InitialHeight, InitialHeight + TargetHeight, CurveValue);

	float VelocityZ = (DesiredHeight - CurrentHeight) / GetWorld()->DeltaTimeSeconds;

	if (VelocityZ > 0) prevVeloJump = FMath::FInterpTo(prevVeloJump, VelocityZ, GetWorld()->DeltaTimeSeconds, 8.0f);

	CurVelocity.Z = prevVeloJump;
}

void UFFMovementBehavior::Slide(const bool IsSlide, const float SlideMultiply, const float MaxSlideSpeed, const UCurveFloat* Curve, float MaxTime, FVector SlopeNormal, EInUseStatusOutputPin& OutputPins)
{
	if (!IsMovementReady()) return;
	if (!Curve) return;

	float dot;
	dot = FVector::DotProduct(SlopeNormal, FVector(0, 0, 1));
	dot = FMath::Abs(dot);
	OutputPins = EInUseStatusOutputPin::NotInUse;

	if (!IsSlide)
	{
		OutputPins = EInUseStatusOutputPin::NotInUse;
		SlideDir = CurVelocity;
		SlideDir.Z = 0;
		SlideTimer = 0;
		return;
	}
	else if (MaxTime < SlideTimer)
	{
		OutputPins = EInUseStatusOutputPin::NotInUse;
		return;
	}

	OutputPins = EInUseStatusOutputPin::InUse;

	FVector SlopeDir = FVector::CrossProduct(SlopeNormal, FVector(0, 0, 1));
	SlopeDir = FVector::CrossProduct(SlopeNormal, SlopeDir);
	if (dot < 1 && SlopeDir.Z < 0) SlopeDir *= -1;

	FVector ProjectedSlideDir = SlideDir - FVector::DotProduct(SlideDir, SlopeNormal.GetSafeNormal()) * SlopeNormal.GetSafeNormal();

	SlideTimer += GetWorld()->DeltaTimeSeconds;

	float Value0To1 = FMath::Clamp(SlideTimer / MaxTime, 0, 1);
	float CurveEval = Curve->GetFloatValue(Value0To1);
	CurveEval = FMath::Lerp(1, SlideMultiply, CurveEval);

	float dotDown = FVector::DotProduct(SlopeNormal, SlideDir);
	if (dot != 1 && dotDown > 0)
	{
		CurveEval = SlideMultiply;
		SlideTimer = 0;
	}

	if (dot == 1) ProjectedSlideDir = SlideDir;

	float StoredZ = CurVelocity.Z;

	CurVelocity = ProjectedSlideDir * CurveEval;
	CurVelocity = CurVelocity.GetClampedToMaxSize(MaxSlideSpeed);
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
		FVector Dir = FVector(VeloToGive.X, VeloToGive.Y, 0);
		float Magnitude = Dir.Length();
		Dir = FVector::VectorPlaneProject(Dir, GroundNormal);
		Dir = VeloToGive.GetSafeNormal() * Magnitude;
		if (VeloToGive.Z > 0) Dir.Z = VeloToGive.Z;
		else Dir.Z += VeloToGive.Z;

		VeloToGive = Dir;
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
		float DotProduct = FVector::DotProduct(FVector(0, 0, 1), HitNormal);

		if (VeloToGive.Z <= 0 && FMath::Abs(DotProduct) >= 0.1)
		{
			FVector Dir = FVector(VeloToGive.X, VeloToGive.Y, 0);
			float Magnitude = Dir.Length();
			Dir = FVector::VectorPlaneProject(Dir, HitNormal);
			Dir = VeloToGive.GetSafeNormal() * Magnitude;
			if (VeloToGive.Z > 0) Dir.Z = VeloToGive.Z;
			else Dir.Z += VeloToGive.Z;

			VeloToGive = Dir;
		}
		else
		{
			VeloToGive = FVector(0, 0, VeloToGive.Z);
		}
	}

	ObjectToMove->SetPhysicsLinearVelocity(VeloToGive);
}

void UFFMovementBehavior::AddExternalForce(FVector Force)
{
	AwaitingForce.Add(Force);
}