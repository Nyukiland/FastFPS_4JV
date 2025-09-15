#include "ComposedProjectile.h"
#include "CapacityEffect.h"

void AComposedProjectile::NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, 
	bool bSelfMoved, FVector HitLocation, FVector HitNormal, 
	FVector NormalImpulse, const FHitResult& Hit)
{
	if (OnHitCapacity)
	{
		OnHitCapacity->ApplyEffect(this, Other);
	}
}
