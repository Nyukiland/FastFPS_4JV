// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "FFNavMeshEnemy.generated.h"

USTRUCT(BlueprintType)
struct FBoundingBox
{
	GENERATED_BODY()

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
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "NavMeshEnemy")
	UBoxComponent* NavBox;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "NavMeshEnemy")
	TArray<FBoundingBox> BoundBoxes;

private:
	void ConnectNeighbor();
	void DrawDebug();

	virtual bool ShouldTickIfViewportsOnly() const override { return true; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "NavMeshEnemy")
	bool CheckValidity();

	UFUNCTION(CallInEditor, BlueprintCallable, Category = "NavMeshEnemy")
	void GenerateNavMesh();

public:
	AFFNavMeshEnemy();
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
