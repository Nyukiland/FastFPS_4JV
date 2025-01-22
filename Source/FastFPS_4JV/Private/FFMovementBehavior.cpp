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

	// ...

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


// Called every frame
void UFFMovementBehavior::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
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
	CurVelocity = ObjectToMove->GetPhysicsLinearVelocity();
}

void UFFMovementBehavior::MoveInDirection(const FVector2D Direction, const float Acceleration, const float Deceleration, const float MaxSpeed)
{
	if (!IsMovementReady()) return;

	float SpeedToGo = Direction.Length() * MaxSpeed;
	float CurSpeed = CurVelocity.Length();

	if (SpeedToGo > CurSpeed) CurSpeed += Acceleration;
	else CurSpeed -= Deceleration;

	CurSpeed = FMath::Clamp(CurSpeed, 0, MaxSpeed);

	FVector NewVelo = ObjectTransformMovement->GetForwardVector() * Direction.X;
	NewVelo += ObjectTransformMovement->GetRightVector() * Direction.Y;
	NewVelo.Z = 0;
	NewVelo = NewVelo.GetSafeNormal();
	NewVelo *= CurSpeed;

	CurVelocity = FVector(NewVelo.X, NewVelo.Y, CurVelocity.Z);
}

void UFFMovementBehavior::MoveInAir(const FVector2D Direction, const float Acceleration, const float Deceleration, const float MaxSpeed)
{
	if (!IsMovementReady()) return;

	FVector2D dir = Direction;
	float VeloZ = CurVelocity.Z;

	dir *= Acceleration;

	FVector ForwardDir = ObjectTransformMovement->GetForwardVector();
	ForwardDir.Z = 0;
	FVector RightDir = ObjectTransformMovement->GetRightVector();
	RightDir.Z = 0;
	FVector NewVelo = ForwardDir.GetSafeNormal() * dir.X;
	NewVelo += RightDir.GetSafeNormal() * dir.Y;

	CurVelocity += NewVelo;
	CurVelocity.Z = 0;
	CurVelocity = CurVelocity.GetClampedToSize(0, MaxSpeed);
	CurVelocity.Z = VeloZ;
}

void UFFMovementBehavior::IsGrounded(FHitResult& GroundHit, float TraceSize, EGroundStatusOutputPin& OutputPins)
{
	if (!IsMovementReady()) return;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(GetOwner());

	FVector VectorDown = ObjectToMove->GetUpVector();
	VectorDown *= TraceSize * -1;

	bool bHit = GetWorld()->LineTraceSingleByChannel(GroundHit, ObjectToMove->GetComponentLocation(), ObjectToMove->GetComponentLocation() + VectorDown, ECC_Visibility, QueryParams);

	OutputPins = bHit ? EGroundStatusOutputPin::Grounded : EGroundStatusOutputPin::NotGrounded;
}

void UFFMovementBehavior::JumpBehavior(const bool Jumped, const float JumpForce, const UCurveFloat* Curve, float MaxTime, bool& InJump)
{
	InJump = false;
	if (!IsMovementReady()) return;

	if (!Jumped)
	{
		JumpTimer = 0;
		JumpDoOnce = false;
		return;
	}
	else if (MaxTime < JumpTimer) return;

	if (!JumpDoOnce)
	{
		JumpDoOnce = true;
		CurVelocity.Z = 0;
	}

	InJump = true;

	JumpTimer += GetWorld()->DeltaTimeSeconds;
	float Value0to1 = FMath::Clamp(JumpTimer / MaxTime, 0, 1);
	float ForceUp = Curve->GetFloatValue(Value0to1);
	AddExternalForce(FVector(0, 0, ForceUp * JumpForce));
}

void UFFMovementBehavior::Slide(const bool IsSlide, const float SlideMultiply, const UCurveFloat* Curve, float MaxTime, bool IsInSlope)
{
	if (!IsMovementReady()) return;

	if (!IsSlide)
	{
		SlideDir = CurVelocity;
		SlideDir.Z = 0;
		SlideTimer = 0;
		return;
	}
	else if (MaxTime < SlideTimer && !IsInSlope) return;

	SlideTimer += GetWorld()->DeltaTimeSeconds;

	float Value0To1 = FMath::Clamp(SlideTimer / MaxTime, 0, 1);
	float CurveEval = Curve->GetFloatValue(Value0To1);
	CurveEval = FMath::Lerp(CurveEval, 1, SlideMultiply);

	float StoredZ = CurVelocity.Z;

	CurVelocity = SlideDir * CurveEval;
	CurVelocity.Z = StoredZ;
}

void UFFMovementBehavior::GiveVelocity()
{
	if (!IsMovementReady()) return;

	if (AwaitingForce.Num() > 0)
	{
		for (const FVector Force : AwaitingForce)
		{
			FVector ModifiedForce = Force; //FVector(Force.X, Force.Y, 0);

			//if (FMath::Abs(Force.Z) > FMath::Abs(CurVelocity.Z)) CurVelocity.Z = Force.Z;
			CurVelocity += ModifiedForce;
		}

		AwaitingForce.Empty();
	}

	ObjectToMove->SetPhysicsLinearVelocity(CurVelocity);
}

void UFFMovementBehavior::Gravity(const float Gravity)
{
	AddExternalForce(FVector(0, 0, -Gravity));
}

void UFFMovementBehavior::AddExternalForce(FVector Force)
{
	AwaitingForce.Add(Force);
}