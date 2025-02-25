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

#pragma region NavMeshCreation

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

	TSet<FVector> InvalidCells;

	// Compute invalid cells based on obstacle positions
	for (const FBox& Obstacle : Obstacles)
	{
		FVector ObstacleMin = Obstacle.Min;
		FVector ObstacleMax = Obstacle.Max;

		for (float x = ObstacleMin.X; x <= ObstacleMax.X; x += GridSize.X * 0.5f)
		{
			for (float y = ObstacleMin.Y; y <= ObstacleMax.Y; y += GridSize.Y * 0.5f)
			{
				for (float z = ObstacleMin.Z; z <= ObstacleMax.Z; z += GridSize.Z * 0.5f)
				{
					FVector CellPos = FVector(
						FMath::GridSnap(x, GridSize.X),
						FMath::GridSnap(y, GridSize.Y),
						FMath::GridSnap(z, GridSize.Z)
					);
					InvalidCells.Add(CellPos);
				}
			}
		}
	}

	// Generate valid navigation boxes
	for (float x = NavBounds.Min.X; x < NavBounds.Max.X; x += GridSize.X)
	{
		for (float y = NavBounds.Min.Y; y < NavBounds.Max.Y; y += GridSize.Y)
		{
			for (float z = NavBounds.Min.Z; z < NavBounds.Max.Z; z += GridSize.Z)
			{
				FVector GridPos = FVector(
					FMath::GridSnap(x, GridSize.X),
					FMath::GridSnap(y, GridSize.Y),
					FMath::GridSnap(z, GridSize.Z)
				);

				if (!InvalidCells.Contains(GridPos))
				{
					FVector Min = GridPos;
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

#pragma endregion

void AFFNavMeshEnemy::DrawDebug()
{
	if (!CheckValidity()) return;

	for (const FBoundingBox Box : BoundBoxes)
	{
		DrawDebugBox(GetWorld(), Box.BoundBox.GetCenter(), Box.BoundBox.GetExtent(), FColor::Green, false, 10, 0, 2);
	}
}

void AFFNavMeshEnemy::GenerateNavMesh()
{
	BoundBoxes.Empty();

	TArray<FBox> Obstacles = CollectObstacles();
	GenerateGrid(Obstacles);
}

void AFFNavMeshEnemy::TestPathfinding()
{
	if (BoundBoxes.Num() < 2) return;

	FBoundingBox* StartBox = &BoundBoxes[0];
	FBoundingBox* EndBox = &BoundBoxes.Last();

	TArray<FVector> Path = FindPath(StartBox->BoundBox.GetCenter(), EndBox->BoundBox.GetCenter());

	UE_LOG(LogTemp, Warning, TEXT("Path found with %d steps"), Path.Num());

	for (int i = 0; i < Path.Num(); i++)
	{
		DrawDebugSphere(GetWorld(), Path[i], 20.0f, 12, FColor::Blue, false, 5.0f, 0, 3);

		if (i > 0)
		{
			DrawDebugLine(GetWorld(), Path[i - 1], Path[i], FColor::Red, false, 5.0f, 0, 2);
		}
	}
}

TArray<FVector> AFFNavMeshEnemy::FindPath(FVector StartPos, FVector EndPos)
{
	TArray<FVector> Path;
	if (BoundBoxes.Num() == 0) return Path;

	// Find the closest bounding boxes to Start and End positions
	FBoundingBox* StartBox = nullptr;
	FBoundingBox* EndBox = nullptr;
	float MinStartDist = FLT_MAX;
	float MinEndDist = FLT_MAX;

	for (FBoundingBox& Box : BoundBoxes)
	{
		float StartDist = FVector::Dist(StartPos, Box.BoundBox.GetCenter());
		float EndDist = FVector::Dist(EndPos, Box.BoundBox.GetCenter());

		if (StartDist < MinStartDist)
		{
			MinStartDist = StartDist;
			StartBox = &Box;
		}

		if (EndDist < MinEndDist)
		{
			MinEndDist = EndDist;
			EndBox = &Box;
		}
	}

	if (!StartBox || !EndBox) return Path; // Return empty if we can't find valid boxes

	// A* Pathfinding Setup
	TSet<FBoundingBox*> ClosedSet;
	TArray<FPathNode*> OpenSet;
	OpenSet.Add(new FPathNode(StartBox, 0, FVector::Dist(StartBox->BoundBox.GetCenter(), EndBox->BoundBox.GetCenter()), nullptr));

	while (OpenSet.Num() > 0)
	{
		OpenSet.Sort([](const FPathNode& A, const FPathNode& B) { return A.FCost() < B.FCost(); });
		FPathNode* CurrentNode = OpenSet[0];
		OpenSet.RemoveAt(0);

		if (CurrentNode->Box == EndBox)
		{
			while (CurrentNode)
			{
				Path.Insert(CurrentNode->Box->BoundBox.GetCenter(), 0);
				CurrentNode = CurrentNode->Parent;
			}
			break;
		}

		ClosedSet.Add(CurrentNode->Box);

		for (FBoundingBox* Neighbor : CurrentNode->Box->Neighbors)
		{
			if (ClosedSet.Contains(Neighbor)) continue;

			float NewGCost = CurrentNode->GCost + FVector::Dist(CurrentNode->Box->BoundBox.GetCenter(), Neighbor->BoundBox.GetCenter());

			FPathNode* ExistingNode = nullptr;
			for (FPathNode* Node : OpenSet)
			{
				if (Node->Box == Neighbor)
				{
					ExistingNode = Node;
					break;
				}
			}

			if (!ExistingNode || NewGCost < ExistingNode->GCost)
			{
				float HCost = FVector::Dist(Neighbor->BoundBox.GetCenter(), EndBox->BoundBox.GetCenter());
				FPathNode* NewNode = new FPathNode(Neighbor, NewGCost, HCost, CurrentNode);

				if (!ExistingNode) OpenSet.Add(NewNode);
			}
		}
	}

	// Clean up memory
	for (FPathNode* Node : OpenSet) delete Node;

	return Path;
}