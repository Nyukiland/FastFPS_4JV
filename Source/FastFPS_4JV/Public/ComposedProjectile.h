#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ComposedProjectile.generated.h"

class UCapacityEffect;

UCLASS()
class FASTFPS_4JV_API AComposedProjectile : public AActor
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, Instanced, BlueprintReadOnly)
	UCapacityEffect* OnHitCapacity;

protected:
	virtual void NotifyHit(UPrimitiveComponent* MyComp, AActor* Other,
		UPrimitiveComponent* OtherComp, bool bSelfMoved,
		FVector HitLocation, FVector HitNormal,
		FVector NormalImpulse, const FHitResult& Hit) override;
};
