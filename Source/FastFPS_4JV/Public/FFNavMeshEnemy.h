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

USTRUCT()
struct FPathNode
{
	GENERATED_BODY()

	FBoundingBox* Box;
	float GCost; 
	float HCost; 
	float FCost() const { return GCost + HCost; }
	FPathNode* Parent;

	FPathNode() {}
	FPathNode(FBoundingBox* InBox, float InGCost, float InHCost, FPathNode* InParent)
		: Box(InBox), GCost(InGCost), HCost(InHCost), Parent(InParent) {
	}
};

UCLASS()
class FASTFPS_4JV_API AFFNavMeshEnemy : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "ANavMeshEnemy")
	UBoxComponent* NavBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ANavMeshEnemy")
	TArray<FBoundingBox> BoundBoxes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ANavMeshEnemy")
	float BoxSize = 50;

private:
	void ConnectNeighbor();

	TArray<FBox> CollectObstacles();

	void GenerateGrid(TArray<FBox> Obstacles);

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "NavMeshEnemy")
	bool CheckValidity();

	UFUNCTION(CallInEditor, BlueprintCallable, Category = "NavMeshEnemy")
	void GenerateNavMesh();

	UFUNCTION(CallInEditor, BlueprintCallable, Category = "NavMeshEnemy")
	void DrawDebug();

	UFUNCTION(CallInEditor, BlueprintCallable, Category = "NavMeshEnemy")
	void TestPathfinding();

public:
	AFFNavMeshEnemy();

	UFUNCTION(BlueprintCallable, Category = "NavMeshEnemy")
	TArray<FVector> FindPath(FVector StartPos, FVector EndPos);

};