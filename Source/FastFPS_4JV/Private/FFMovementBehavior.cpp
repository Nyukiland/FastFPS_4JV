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

	UE_LOG(LogTemp, Error, TEXT("Velo: %s"), *dir.ToString());

	FVector NewVelo = ObjectTransformMovement->GetForwardVector() * dir.X;
	NewVelo += ObjectTransformMovement->GetRightVector() * dir.Y;
	NewVelo.Z = 0;

	UE_LOG(LogTemp, Error, TEXT("Velo: %s"), *NewVelo.ToString());
	UE_LOG(LogTemp, Error, TEXT("____________________"));

	CurVelocity += NewVelo;
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

void UFFMovementBehavior::Jump(const float JumpForce)
{
	if (!IsMovementReady()) return;

	AwaitingForce.Add(FVector(0, 0, JumpForce));
}

void UFFMovementBehavior::Slide(const bool IsSlide, const float SlideMultiply, const UCurveFloat* Curve, float MaxTime, bool isInSlope)
{
	if (!IsMovementReady()) return;

	if (IsSlide && SlideTimer == MaxTime)
	{
		SlideTimer = 0;
	}
	else if (!IsSlide)
	{
		SlideTimer = MaxTime;
		return;
	}

	if (SlideTimer == MaxTime && !isInSlope) return;

	SlideTimer += GetWorld()->DeltaTimeSeconds;

	float Value0To1 = FMath::Clamp(SlideTimer / MaxTime, 0, 1);
	float CurveEval = Curve->GetFloatValue(Value0To1);
	FVector Movement = FVector(CurVelocity.X, CurVelocity.Y, 0);

	float StoredZ = CurVelocity.Z;

	CurVelocity = Movement * CurveEval * SlideMultiply;
	CurVelocity.Z = StoredZ;
}

void UFFMovementBehavior::GiveVelocity()
{
	if (!IsMovementReady()) return;

	if (AwaitingForce.Num() > 0)
	{
		for (const FVector Force : AwaitingForce)
		{
			FVector ModifiedForce = FVector(Force.X, Force.Y, 0);

			if (FMath::Abs(Force.Z) > FMath::Abs(CurVelocity.Z)) CurVelocity.Z = Force.Z;
			CurVelocity += ModifiedForce;
		}

		AwaitingForce.Empty();
	}

	ObjectToMove->SetPhysicsLinearVelocity(CurVelocity);
}

void UFFMovementBehavior::AddExternalForce(FVector Force)
{
	AwaitingForce.Add(Force);
}