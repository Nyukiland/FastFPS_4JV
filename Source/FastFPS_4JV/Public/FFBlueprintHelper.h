// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "FFBlueprintHelper.generated.h"

UENUM()
enum class EKeyboardOutputPin : uint8
{
	FrenchKeyboard,
	EnglishKeyboard,
	Unknown
};

UCLASS()
class FASTFPS_4JV_API UFFBlueprintHelper : public UObject
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "Keyboard", meta = (ExpandEnumAsExecs = "OutputPins"))
	static void CheckCurrentLayout(EKeyboardOutputPin& OutputPins);

	UFUNCTION(BlueprintCallable, Category = "Curve")
	static float EvaluateCurve(const UCurveFloat* Curve, const float Value);
};
