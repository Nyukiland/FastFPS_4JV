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
