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

	void BuildAllLayers();
	void BuildLayer(int32 LayerIndex, int32 SectionIndex);
	FVector SampleVertexForLayer(int32 LayerIndex, int32 thetaIndex, int32 phiIndex) const;

	// math helpers (from the p5 sketches)
	float VShape(float A, float r, float a, float b, float c) const;
	float Bumpiness(float A, float r, float f, float angleDeg) const;

	// skip degenerate triangles helper
	bool IsTriangleDegenerate(const FVector& A, const FVector& B, const FVector& C) const;

public:
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Flower")
	void RandomizeFlower();

public:
	// Visual mesh
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UProceduralMeshComponent* ProcMesh;

	// Global grid resolution (theta = rows, phi = cols)
	UPROPERTY(EditAnywhere, Category = "Flower", meta = (ClampMin = "4", UIMin = "4"))
	int32 Rows = 60;

	UPROPERTY(EditAnywhere, Category = "Flower", meta = (ClampMin = "4", UIMin = "4"))
	int32 Cols = 120;

	// Layers: each layer can be thought of as one "flower" shell (use multiple for double/triple)
	UPROPERTY(EditAnywhere, Category = "Flower", meta = (ClampMin = "1", UIMin = "1"))
	int32 LayerCount = 2;

	// Per-layer arrays (size LayerCount). If you change LayerCount in editor, set sensible defaults.
	UPROPERTY(EditAnywhere, Category = "Flower")
	TArray<int32> LayerPetalNum;

	UPROPERTY(EditAnywhere, Category = "Flower")
	TArray<float> LayerDiameter;

	UPROPERTY(EditAnywhere, Category = "Flower")
	TArray<float> LayerPetalLength;

	UPROPERTY(EditAnywhere, Category = "Flower")
	TArray<float> LayerPetalSharpness;

	UPROPERTY(EditAnywhere, Category = "Flower")
	TArray<float> LayerFlowerHeight;

	UPROPERTY(EditAnywhere, Category = "Flower")
	TArray<float> LayerCurvature1;

	UPROPERTY(EditAnywhere, Category = "Flower")
	TArray<float> LayerCurvature2;

	UPROPERTY(EditAnywhere, Category = "Flower")
	TArray<float> LayerBump;

	UPROPERTY(EditAnywhere, Category = "Flower")
	TArray<int32> LayerBumpNum;

	// Layer transform modifiers
	UPROPERTY(EditAnywhere, Category = "Flower")
	TArray<float> LayerScale; // scale per layer

	UPROPERTY(EditAnywhere, Category = "Flower")
	TArray<float> LayerRotationDeg; // rotation offset around Z per layer

	// Rendering
	UPROPERTY(EditAnywhere, Category = "Flower")
	UMaterialInterface* LayerMaterial;

	// Whether to auto-disable collision for the generated mesh (recommended true)
	UPROPERTY(EditAnywhere, Category = "Flower")
	bool bDisableCollision = true;
};