// Fill out your copyright notice in the Description page of Project Settings.


#include "FFBlueprintHelper.h"
#include "Windows/AllowWindowsPlatformTypes.h"
#include <Windows.h>
#include "Windows/HideWindowsPlatformTypes.h"

void UFFBlueprintHelper::CheckCurrentLayout(EKeyboardOutputPin& OutputPins)
{
	// Get the current keyboard layout
	HKL KeyboardLayout = GetKeyboardLayout(0); // Get layout for the active thread
	TCHAR Buffer[KL_NAMELENGTH];

	if (GetKeyboardLayoutName(Buffer))
	{
		FString LayoutName(Buffer);

		if (LayoutName == "00000409") // US QWERTY
		{
			OutputPins = EKeyboardOutputPin::EnglishKeyboard;
		}
		else if (LayoutName == "0000040C") // French AZERTY
		{
			OutputPins = EKeyboardOutputPin::FrenchKeyboard;
		}
		else
		{
			OutputPins = EKeyboardOutputPin::Unknown;
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to retrieve keyboard layout name"));
	}
}

float UFFBlueprintHelper::EvaluateCurve(const UCurveFloat* Curve, const float Value)
{
	return Curve->GetFloatValue(Value);
}

float& UFFBlueprintHelper::SmoothValueByRef(UPARAM(ref) float& CurValue, const float ValueToReach, const float SmoothSpeed, const float DeltaTime)
{
	float T = SmoothSpeed * DeltaTime;
	T = FMath::Clamp(T, 0, 1);

	CurValue = FMath::Lerp(CurValue, ValueToReach, T);
	return CurValue;
}

bool UFFBlueprintHelper::IsInSight(const AActor* Owner, const FVector StartPoint, const FVector EndPoint, const AActor* EndActor)
{
	if (!Owner || !Owner->GetWorld())
	{
		return false;
	}

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(Owner);

	FHitResult Hit;

	bool bHit = Owner->GetWorld()->LineTraceSingleByChannel(Hit, StartPoint, EndPoint, ECC_Visibility, QueryParams);

	if (!EndActor || !bHit) return !bHit;
	else return EndActor == Hit.GetActor();

	return false; 
}