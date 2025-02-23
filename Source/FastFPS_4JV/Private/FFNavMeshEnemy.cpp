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

	FVector BoxExtent = FVector(50, 50, 50); // Half-size of each grid cell

	FVector NavBoxMin = NavBox->Bounds.GetBox().Min;
	FVector NavBoxMax = NavBox->Bounds.GetBox().Max;

	int NumX = FMath::FloorToInt((NavBoxMax.X - NavBoxMin.X) / BoxeSize);
	int NumY = FMath::FloorToInt((NavBoxMax.Y - NavBoxMin.Y) / BoxeSize);
	int NumZ = FMath::FloorToInt((NavBoxMax.Z - NavBoxMin.Z) / BoxeSize);

	for (int x = 0; x < NumX; x++)
	{
		for (int y = 0; y < NumY; y++)
		{
			for (int z = 0; z < NumZ; z++)
			{
				FVector CellMin = NavBoxMin + FVector(x * BoxeSize, y * BoxeSize, z * BoxeSize);
				FVector CellMax = CellMin + FVector(BoxeSize, BoxeSize, BoxeSize);
				FBox CellBox(CellMin, CellMax);

				bool bBlocked = false;
				for (const FBox& ObstacleBox : Obstacles)
				{
					if (CellBox.Intersect(ObstacleBox))
					{
						bBlocked = true;
						break;
					}
				}

				if (!bBlocked)
				{
					BoundBoxes.Add(FBoundingBox(CellMin, CellMax));
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

