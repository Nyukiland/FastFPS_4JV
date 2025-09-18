#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CapacityEffect.generated.h"

class UStatContainer;

UCLASS(Abstract, Blueprintable, EditInlineNew, DefaultToInstanced)
class FASTFPS_4JV_API UCapacityEffect : public UObject
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintNativeEvent, Category = "Capacity|Effect")
	void ApplyEffect(AActor* Source, AActor* Target);
	virtual void ApplyEffect_Implementation(AActor* Source, AActor* Target);

	UFUNCTION(BlueprintNativeEvent, Category = "Capacity|Effect")
	TArray<UStatContainer*> GetStats();
	virtual TArray<UStatContainer*> GetStats_Implementation();
};