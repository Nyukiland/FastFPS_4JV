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

	FVector NewVelo;
	if (Direction != FVector2D(0,0))
	{
		CurSpeed += Acceleration;
		CurSpeed = FMath::Clamp(CurSpeed, 0, MaxSpeed);

		NewVelo = ObjectTransformMovement->GetForwardVector() * Direction.X;
		NewVelo += ObjectTransformMovement->GetRightVector() * Direction.Y;
		NewVelo.Z = 0;
		NewVelo = NewVelo.GetSafeNormal();
		NewVelo *= CurSpeed;
	}
	else
	{
		CurSpeed -= Acceleration;
		CurSpeed = FMath::Clamp(CurSpeed, 0, MaxSpeed);

		NewVelo = CurVelocity;
		NewVelo.Z = 0;
		NewVelo = NewVelo.GetSafeNormal();
		NewVelo *= CurSpeed;

	}

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

void UFFMovementBehavior::JumpBehavior(const float JumpForce, const UCurveFloat* Curve, const float MaxTime, const float Timer)
{
	if (!IsMovementReady()) return;

	float Value0to1 = FMath::Clamp(Timer / MaxTime, 0, 1);
	float ForceUp = Curve->GetFloatValue(Value0to1);
	AddExternalForce(FVector(0, 0, ForceUp * JumpForce));
}

void UFFMovementBehavior::Slide(const bool IsSlide, const float SlideMultiply, const UCurveFloat* Curve, float MaxTime, FVector SlopeNormal)
{
	if (!IsMovementReady()) return;
	if (!Curve) return;

	float dot;
	dot = FVector::DotProduct(SlopeNormal, FVector(0, 0, 1));
	dot = FMath::Abs(dot);

	if (!IsSlide)
	{
		SlideDir = CurVelocity;
		SlideDir.Z = 0;
		SlideTimer = 0;
		return;
	}
	else if (MaxTime < SlideTimer) return;

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