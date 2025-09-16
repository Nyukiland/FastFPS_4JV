#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "ProceduralFlower.generated.h"

UCLASS()
class FASTFPS_4JV_API AProceduralFlower : public AActor
{
	GENERATED_BODY()

public:
	AProceduralFlower();

protected:
	virtual void OnConstruction(const FTransform& Transform) override;

	void BuildFlower();

	float VShape(float A, float r, float a, float b, float c) const;
	float Bumpiness(float A, float r, float f, float angle) const;

public:
	UPROPERTY(VisibleAnywhere)
	UProceduralMeshComponent* ProcMesh;

	// Parameters directly adapted from the JS sliders
	UPROPERTY(EditAnywhere, Category = "Flower")
	int32 Rows = 60;

	UPROPERTY(EditAnywhere, Category = "Flower")
	int32 Cols = 120;

	UPROPERTY(EditAnywhere, Category = "Flower")
	int32 PetalNum = 5;

	UPROPERTY(EditAnywhere, Category = "Flower")
	float Diameter = 200.f;

	UPROPERTY(EditAnywhere, Category = "Flower")
	float PetalLength = 60.f;

	UPROPERTY(EditAnywhere, Category = "Flower")
	float PetalSharpness = 0.4f;

	UPROPERTY(EditAnywhere, Category = "Flower")
	float FlowerHeight = 300.f;

	UPROPERTY(EditAnywhere, Category = "Flower")
	float Curvature1 = 0.8f;

	UPROPERTY(EditAnywhere, Category = "Flower")
	float Curvature2 = 0.2f;

	UPROPERTY(EditAnywhere, Category = "Flower")
	float Bump = 2.5f;

	UPROPERTY(EditAnywhere, Category = "Flower")
	int32 BumpNum = 10;
};
