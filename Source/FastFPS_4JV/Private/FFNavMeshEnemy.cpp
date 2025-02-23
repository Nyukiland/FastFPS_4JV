#include "FFNavMeshEnemy.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "CollisionQueryParams.h"
#include "Engine/OverlapResult.h"

// Sets default values
AFFNavMeshEnemy::AFFNavMeshEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	NavBox = CreateDefaultSubobject<UBoxComponent>(TEXT("NavBox"));
	NavBox->SetBoxExtent(FVector(100, 100, 100));
	NavBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RootComponent = NavBox;
}

TArray<FBox> AFFNavMeshEnemy::CollectObstacles()
{
	TArray<FBox> Obstacles;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(NavBox->GetOwner());

	TArray<FOverlapResult> Overlaps;
	FCollisionShape BoxShape = FCollisionShape::MakeBox(NavBox->GetScaledBoxExtent());

	bool bHasOverlaps = GetWorld()->OverlapMultiByObjectType(
		Overlaps,
		NavBox->GetComponentLocation(),
		FQuat::Identity,
		FCollisionObjectQueryParams::AllObjects, // Detect static objects
		BoxShape,
		QueryParams
	);

	UE_LOG(LogTemp, Warning, TEXT("Obstacles Found: %i"), Overlaps.Num());

	if (bHasOverlaps)
	{
		for (const FOverlapResult& Result : Overlaps)
		{
			AActor* OverlappingActor = Result.GetActor();
			if (OverlappingActor)
			{
				FVector Origin, Extent;
				OverlappingActor->GetActorBounds(true, Origin, Extent);
				FBox ObstacleBox(Origin - Extent, Origin + Extent);
				Obstacles.Add(ObstacleBox);
			}
		}
	}

	return Obstacles;
}

void AFFNavMeshEnemy::GenerateGrid(TArray<FBox> Obstacles)
{
	BoundBoxes.Empty();

	FVector GridSize = FVector(BoxSize, BoxSize, BoxSize);
	FBox NavBounds = NavBox->Bounds.GetBox();

	TSet<FIntVector> InvalidCells;

	// Convert navigation bounds to grid indices
	FIntVector GridMin = FIntVector(
		FMath::FloorToInt(NavBounds.Min.X / GridSize.X),
		FMath::FloorToInt(NavBounds.Min.Y / GridSize.Y),
		FMath::FloorToInt(NavBounds.Min.Z / GridSize.Z)
	);

	FIntVector GridMax = FIntVector(
		FMath::CeilToInt(NavBounds.Max.X / GridSize.X),
		FMath::CeilToInt(NavBounds.Max.Y / GridSize.Y),
		FMath::CeilToInt(NavBounds.Max.Z / GridSize.Z)
	);

	//Directly Compute Invalid Cells
	for (const FBox& Obstacle : Obstacles)
	{
		FIntVector ObstacleMinIndex = FIntVector(
			FMath::FloorToInt(Obstacle.Min.X / GridSize.X),
			FMath::FloorToInt(Obstacle.Min.Y / GridSize.Y),
			FMath::FloorToInt(Obstacle.Min.Z / GridSize.Z)
		);

		FIntVector ObstacleMaxIndex = FIntVector(
			FMath::CeilToInt(Obstacle.Max.X / GridSize.X),
			FMath::CeilToInt(Obstacle.Max.Y / GridSize.Y),
			FMath::CeilToInt(Obstacle.Max.Z / GridSize.Z)
		);

		// Mark all grid cells occupied by the obstacle as invalid
		for (int x = ObstacleMinIndex.X; x <= ObstacleMaxIndex.X; x++)
		{
			for (int y = ObstacleMinIndex.Y; y <= ObstacleMaxIndex.Y; y++)
			{
				for (int z = ObstacleMinIndex.Z; z <= ObstacleMaxIndex.Z; z++)
				{
					InvalidCells.Add(FIntVector(x, y, z));
				}
			}
		}
	}

	//Directly Generate Valid Bounding Boxes
	for (int x = GridMin.X; x < GridMax.X; x++)
	{
		for (int y = GridMin.Y; y < GridMax.Y; y++)
		{
			for (int z = GridMin.Z; z < GridMax.Z; z++)
			{
				FIntVector GridPos(x, y, z);

				if (!InvalidCells.Contains(GridPos)) // Add only valid boxes
				{
					FVector Min = FVector(x * GridSize.X, y * GridSize.Y, z * GridSize.Z);
					FVector Max = Min + GridSize;
					BoundBoxes.Add(FBoundingBox(Min, Max));
				}
			}
		}
	}

	ConnectNeighbor();
}

void AFFNavMeshEnemy::ConnectNeighbor()
{
	for (FBoundingBox& Box : BoundBoxes)
	{
		for (FBoundingBox& OtherBox : BoundBoxes)
		{
			if (&Box == &OtherBox) continue;

			FVector Diff = OtherBox.BoundBox.GetCenter() - Box.BoundBox.GetCenter();
			if (FMath::Abs(Diff.X) <= 100.0f && Diff.Y == 0 && Diff.Z == 0 ||
				FMath::Abs(Diff.Y) <= 100.0f && Diff.X == 0 && Diff.Z == 0 ||
				FMath::Abs(Diff.Z) <= 100.0f && Diff.X == 0 && Diff.Y == 0)
			{
				Box.Neighbors.Add(&OtherBox);
			}
		}
	}
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

	if (BoundBoxes.Num() == 0) GenerateNavMesh();
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

	TArray<FBox> Obstacles = CollectObstacles();
	GenerateGrid(Obstacles);
}

// Called every frame
void AFFNavMeshEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GIsEditor && GetWorld()->WorldType == EWorldType::Editor) DrawDebug();
}

