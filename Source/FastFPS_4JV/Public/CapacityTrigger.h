#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CapacityTrigger.generated.h"

class UStatContainer;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTriggerActivated);

UCLASS(Abstract, Blueprintable, EditInlineNew, DefaultToInstanced)
class FASTFPS_4JV_API UCapacityTrigger : public UObject
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintAssignable)
	FOnTriggerActivated OnTriggered;

	UFUNCTION(BlueprintNativeEvent, Category = "Capacity|Trigger")
	void TickTrigger(float DeltaTime);
	virtual void TickTrigger_Implementation(float DeltaTime);

	UFUNCTION(BlueprintNativeEvent, Category = "Capacity|Trigger")
	void ResetTrigger();
	virtual void ResetTrigger_Implementation();

	UFUNCTION(BlueprintNativeEvent, Category = "Capacity|Effect")
	TArray<UStatContainer*> GetStats();
	virtual TArray<UStatContainer*> GetStats_Implementation();
};
