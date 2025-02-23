#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Engine/EngineTypes.h"
#include "DrawDebugHelpers.h"
#include "FFNavMeshEnemy.generated.h"

USTRUCT(BlueprintType)
struct FBoundingBox
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FBox BoundBox;
	TArray<FBoundingBox*> Neighbors;

	FBoundingBox() {}
	FBoundingBox(const FVector Min, const FVector Max) : BoundBox(Min, Max) {}
};

UCLASS()
class FASTFPS_4JV_API AFFNavMeshEnemy : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "ANavMeshEnemy")
	UBoxComponent* NavBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "ANavMeshEnemy")
	TArray<FBoundingBox> BoundBoxes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ANavMeshEnemy")
	float BoxSize = 50;

private:
	void ConnectNeighbor();
	void DrawDebug();

	// Function to collect obstacles within NavBox
	TArray<FBox> CollectObstacles();

	// Function to generate the grid
	void GenerateGrid(TArray<FBox> Obstacles);

	virtual bool ShouldTickIfViewportsOnly() const override { return true; }

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "ANavMeshEnemy")
	bool CheckValidity();

	UFUNCTION(CallInEditor, BlueprintCallable, Category = "ANavMeshEnemy")
	void GenerateNavMesh();

public:
	AFFNavMeshEnemy();
	virtual void Tick(float DeltaTime) override;
};