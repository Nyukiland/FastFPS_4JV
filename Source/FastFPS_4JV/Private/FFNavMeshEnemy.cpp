// Fill out your copyright notice in the Description page of Project Settings.


#include "FFNavMeshEnemy.h"

// Sets default values
AFFNavMeshEnemy::AFFNavMeshEnemy()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	NavBox = CreateDefaultSubobject<UBoxComponent>(TEXT("NavBox"));
	NavBox->SetBoxExtent(FVector(100, 100, 100));
	NavBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RootComponent = NavBox;
}

void AFFNavMeshEnemy::ConnectNeighbor()
{

}

void AFFNavMeshEnemy::DrawDebug()
{
	if (!IsSelectedInEditor() || !CheckValidity()) return;

	for (const FBoundingBox Box : BoundBoxes)
	{
		DrawDebugBox(GetWorld(), Box.BoundBox.GetCenter(), Box.BoundBox.GetExtent(), FColor::Green, false, -1, 0, 2);
	}
}

// Called when the game starts or when spawned
void AFFNavMeshEnemy::BeginPlay()
{
	Super::BeginPlay();

}

bool AFFNavMeshEnemy::CheckValidity()
{
	if (IsValid(NavBox))
	{
		return true;
	}

	UE_LOG(LogTemp, Error, TEXT("[FFNavMeshEnemy] bounding box is not valid"));
	return false;
}

void AFFNavMeshEnemy::GenerateNavMesh()
{
	BoundBoxes.Empty();

	FBoundingBox newBox = FBoundingBox(NavBox->Bounds.GetBox().Min, NavBox->Bounds.GetBox().Max);

	BoundBoxes.Add(newBox);
}

// Called every frame
void AFFNavMeshEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GIsEditor && GetWorld()->WorldType == EWorldType::Editor) DrawDebug();
}

