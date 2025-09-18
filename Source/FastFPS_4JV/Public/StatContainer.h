#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "StatContainer.generated.h"

USTRUCT(Blueprintable)
struct FStatUpgrade
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float UpgradeValue = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float UpgradePrice = 0;
};

UCLASS(Blueprintable, EditInlineNew, DefaultToInstanced)
class FASTFPS_4JV_API UStatContainer : public UObject
{
	GENERATED_BODY()
	
private:
	int CurrentUpgradeCount = -1;

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FName Name = "Name";

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float BaseValue = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FStatUpgrade> Upgrades;

public:
	UFUNCTION(BlueprintCallable)
	float GetCurrentValue() const 
	{
		float value = BaseValue;

		if (CurrentUpgradeCount >= 0)
			value = Upgrades[CurrentUpgradeCount].UpgradeValue;

		return value;
	}

	UFUNCTION(BlueprintCallable)
	bool CheckIfCanUpgrade(float money);

	UFUNCTION(BlueprintCallable)
	void Upgrade();

	UFUNCTION(BlueprintCallable)
	bool TryUpgrade(float money);
};