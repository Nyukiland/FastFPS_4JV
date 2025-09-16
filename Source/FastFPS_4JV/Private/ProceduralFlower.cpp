#include "ProceduralFlower.h"

AProceduralFlower::AProceduralFlower()
{
	PrimaryActorTick.bCanEverTick = false;

	PrimaryActorTick.bCanEverTick = false;

	ProcMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProcMesh"));
	RootComponent = ProcMesh;
	ProcMesh->bUseAsyncCooking = true;
}

void AProceduralFlower::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	BuildFlower();
}

float AProceduralFlower::VShape(float A, float r, float a, float b, float c) const
{
	return A * FMath::Pow(FMath::Exp(-b * FMath::Pow(FMath::Abs(r), c)), 1.f) * FMath::Pow(FMath::Abs(r), a);
}

float AProceduralFlower::Bumpiness(float A, float r, float f, float angle) const
{
	return 1 + A * FMath::Square(r) * FMath::Sin(f * angle);
}

void AProceduralFlower::BuildFlower()
{
	TArray<FVector> Vertices;
	TArray<int32> Triangles;
	TArray<FVector> Normals;
	TArray<FVector2D> UVs;
	TArray<FProcMeshTangent> Tangents;

	Vertices.SetNum((Rows + 1) * (Cols + 1));
	UVs.SetNum((Rows + 1) * (Cols + 1));
	Normals.SetNum((Rows + 1) * (Cols + 1));
	Tangents.SetNum((Rows + 1) * (Cols + 1));

	// Generate vertices
	for (int theta = 0; theta <= Rows; theta++)
	{
		for (int phi = 0; phi <= Cols; phi++)
		{
			float u = float(theta) / float(Rows);
			float v = float(phi) / float(Cols);

			float angleDeg = v * 360.f;
			float angleRad = FMath::DegreesToRadians(angleDeg);

			float r = (PetalLength * FMath::Pow(FMath::Abs(FMath::Sin(PetalNum / 2.f * angleDeg)), PetalSharpness) + Diameter) * u;

			float x = r * FMath::Cos(angleRad);
			float y = r * FMath::Sin(angleRad);

			float z = VShape(FlowerHeight, r / 100.f, Curvature1, Curvature2, 1.5f)
				- 200.f
				+ Bumpiness(Bump, r / 100.f, BumpNum, angleDeg);

			int idx = theta * (Cols + 1) + phi;
			Vertices[idx] = FVector(x, y, z);
			UVs[idx] = FVector2D(u, v);
		}
	}

	// Triangles
	for (int theta = 0; theta < Rows; theta++)
	{
		for (int phi = 0; phi < Cols; phi++)
		{
			int i0 = theta * (Cols + 1) + phi;
			int i1 = i0 + 1;
			int i2 = (theta + 1) * (Cols + 1) + phi;
			int i3 = i2 + 1;

			Triangles.Add(i0);
			Triangles.Add(i2);
			Triangles.Add(i1);

			Triangles.Add(i1);
			Triangles.Add(i2);
			Triangles.Add(i3);
		}
	}

	// Normals (approximate: point up)
	for (int i = 0; i < Normals.Num(); i++)
	{
		Normals[i] = FVector(0, 0, 1);
		Tangents[i] = FProcMeshTangent(1, 0, 0);
	}

	ProcMesh->ClearAllMeshSections();
	ProcMesh->CreateMeshSection(0, Vertices, Triangles, Normals, UVs, {}, Tangents, false);
}