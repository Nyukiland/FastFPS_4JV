#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CapacityTrigger.generated.h"

class UStatContainer;

UCLASS(Abstract, Blueprintable, EditInlineNew, DefaultToInstanced)
class FASTFPS_4JV_API UCapacityTrigger : public UObject
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Capacity|Trigger")
	bool TriggerReady = false;

public:
	UFUNCTION(BlueprintNativeEvent, Category = "Capacity|Trigger")
	void TickTrigger(float DeltaTime);
	virtual void TickTrigger_Implementation(float DeltaTime);
	
	UFUNCTION(BlueprintNativeEvent, Category = "Capacity|Trigger")
	void ResetTrigger();
	virtual void ResetTrigger_Implementation();

	UFUNCTION(BlueprintNativeEvent, Category = "Capacity|Trigger")
	TArray<UStatContainer*> GetStats();
	virtual TArray<UStatContainer*> GetStats_Implementation();
};
