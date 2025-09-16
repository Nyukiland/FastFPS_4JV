#include "ProceduralFlower.h"
#include "Kismet/KismetMathLibrary.h"

AProceduralFlower::AProceduralFlower()
{
	PrimaryActorTick.bCanEverTick = false;

	ProcMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProcMesh"));
	RootComponent = ProcMesh;
	ProcMesh->bUseAsyncCooking = true;
}

void AProceduralFlower::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	// Ensure arrays have LayerCount size (basic safety/defaulting)
	auto EnsureArraySize = [&](auto& Arr, auto DefaultValue)
		{
			if (Arr.Num() < LayerCount)
			{
				int32 Needed = LayerCount - Arr.Num();
				for (int i = 0; i < Needed; ++i) Arr.Add(DefaultValue);
			}
		};

	EnsureArraySize(LayerPetalNum, 5);
	EnsureArraySize(LayerDiameter, 200.f);
	EnsureArraySize(LayerPetalLength, 60.f);
	EnsureArraySize(LayerPetalSharpness, 0.4f);
	EnsureArraySize(LayerFlowerHeight, 300.f);
	EnsureArraySize(LayerCurvature1, 0.8f);
	EnsureArraySize(LayerCurvature2, 0.2f);
	EnsureArraySize(LayerBump, 2.5f);
	EnsureArraySize(LayerBumpNum, 10);
	EnsureArraySize(LayerScale, 1.f);
	EnsureArraySize(LayerRotationDeg, 0.f);

	BuildAllLayers();
}

float AProceduralFlower::VShape(float A, float r, float a, float b, float c) const
{
	// vShape(A, r, a, b, c) = A * e^{-b * |r|^c} * |r|^a
	float termExp = FMath::Exp(-b * FMath::Pow(FMath::Abs(r), c));
	return A * termExp * FMath::Pow(FMath::Abs(r), a);
}

float AProceduralFlower::Bumpiness(float A, float r, float f, float angleDeg) const
{
	// bumpiness(A, r, f, angle) = 1 + A * r^2 * sin(f * angle)
	// angle in degrees in original; convert to radians for FMath::Sin
	float angleRad = FMath::DegreesToRadians(angleDeg);
	return 1.f + A * FMath::Square(r) * FMath::Sin(f * angleRad);
}

bool AProceduralFlower::IsTriangleDegenerate(const FVector& A, const FVector& B, const FVector& C) const
{
	FVector AB = B - A;
	FVector AC = C - A;
	float areaSq = FVector::CrossProduct(AB, AC).SizeSquared();
	// threshold tuned to reject near-zero area
	return areaSq <= KINDA_SMALL_NUMBER;
}

void AProceduralFlower::RandomizeFlower()
{
	LayerCount = FMath::RandRange(1, 6);

	// Resize arrays
	LayerPetalNum.SetNum(LayerCount);
	LayerDiameter.SetNum(LayerCount);
	LayerPetalLength.SetNum(LayerCount);
	LayerPetalSharpness.SetNum(LayerCount);
	LayerFlowerHeight.SetNum(LayerCount);
	LayerCurvature1.SetNum(LayerCount);
	LayerCurvature2.SetNum(LayerCount);
	LayerBump.SetNum(LayerCount);
	LayerBumpNum.SetNum(LayerCount);
	LayerScale.SetNum(LayerCount);
	LayerRotationDeg.SetNum(LayerCount);

	int BasePetalNum = 8;
	float BaseDiameter = 200.f;
	float BasePetalLength = 80.f;
	float BaseSharpness = 0.1f;
	float BaseHeight = 300.f;

	for (int32 L = 0; L < LayerCount; ++L)
	{
		float LayerRatio = 1.f - (float)L / (float)(LayerCount); 

		LayerPetalNum[L] = BasePetalNum + L * 2;

		LayerDiameter[L] = BaseDiameter * LayerRatio + FMath::FRandRange(-10.f, 10.f);

		LayerPetalLength[L] = BasePetalLength * LayerRatio + FMath::FRandRange(-10.f, 10.f);

		LayerPetalSharpness[L] = BaseSharpness + L * 0.2f;

		LayerFlowerHeight[L] = BaseHeight - L * 50.f;

		LayerCurvature1[L] = 0.8f + FMath::FRandRange(-0.2f, 0.2f);
		LayerCurvature2[L] = 0.2f + FMath::FRandRange(-0.05f, 0.05f);

		LayerBump[L] = 2.f + FMath::FRandRange(-0.5f, 0.5f);
		LayerBumpNum[L] = 10 + L;

		LayerScale[L] = 1.f - (0.2f * L);

		LayerRotationDeg[L] = FMath::FRandRange(0.f, 360.f);
	}

	BuildAllLayers();
}

void AProceduralFlower::BuildAllLayers()
{
	ProcMesh->ClearAllMeshSections();

	// We'll merge all layers into single vertex/triangle buffers, but keep section per layer index for materials
	TArray<FVector> AllVertices;
	TArray<int32> AllTriangles;
	TArray<FVector> AllNormals;
	TArray<FVector2D> AllUVs;
	TArray<FProcMeshTangent> AllTangents;
	TArray<int32> SectionVertexStart; SectionVertexStart.SetNum(LayerCount);

	int32 vertexOffset = 0;
	int32 sectionIndex = 0;

	for (int32 L = 0; L < LayerCount; ++L)
	{
		SectionVertexStart[L] = vertexOffset;

		// Build single layer into local buffers then append
		TArray<FVector> Vertices;
		TArray<int32> Triangles;
		TArray<FVector> Normals;
		TArray<FVector2D> UVs;
		TArray<FProcMeshTangent> Tangents;

		int32 rows = FMath::Max(1, Rows);
		int32 cols = FMath::Max(3, Cols);

		Vertices.SetNum((rows + 1) * (cols + 1));
		UVs.SetNum((rows + 1) * (cols + 1));
		Normals.SetNum((rows + 1) * (cols + 1));
		Tangents.SetNum((rows + 1) * (cols + 1));

		// Sample vertices
		for (int theta = 0; theta <= rows; ++theta)
		{
			float u = float(theta) / float(rows); // theta/rows
			for (int phi = 0; phi <= cols; ++phi)
			{
				int idx = theta * (cols + 1) + phi;
				FVector p = SampleVertexForLayer(L, theta, phi);
				// apply layer-level rotation and scale offsets
				if (!FMath::IsNearlyEqual(LayerRotationDeg[L], 0.f))
				{
					p = p.RotateAngleAxis(LayerRotationDeg[L], FVector(0, 0, 1));
				}
				if (!FMath::IsNearlyEqual(LayerScale[L], 1.f))
				{
					// scale around actor location
					FVector local = p - GetActorLocation();
					local *= LayerScale[L];
					p = GetActorLocation() + local;
				}

				Vertices[idx] = p;
				UVs[idx] = FVector2D(u, float(phi) / float(cols));
			}
		}

		// Make triangles (skip degenerate)
		for (int theta = 0; theta < rows; ++theta)
		{
			for (int phi = 0; phi < cols; ++phi)
			{
				int i0 = theta * (cols + 1) + phi;
				int i1 = i0 + 1;
				int i2 = (theta + 1) * (cols + 1) + phi;
				int i3 = i2 + 1;

				// tri 1
				if (!IsTriangleDegenerate(Vertices[i0], Vertices[i2], Vertices[i1]))
				{
					Triangles.Add(i0);
					Triangles.Add(i2);
					Triangles.Add(i1);
				}

				// tri 2
				if (!IsTriangleDegenerate(Vertices[i1], Vertices[i2], Vertices[i3]))
				{
					Triangles.Add(i1);
					Triangles.Add(i2);
					Triangles.Add(i3);
				}
			}
		}

		// Normals accumulation
		for (int i = 0; i < Normals.Num(); ++i) Normals[i] = FVector::ZeroVector;
		for (int t = 0; t < Triangles.Num(); t += 3)
		{
			int ia = Triangles[t], ib = Triangles[t + 1], ic = Triangles[t + 2];
			FVector A = Vertices[ia], B = Vertices[ib], C = Vertices[ic];
			FVector triN = FVector::CrossProduct(B - A, C - A).GetSafeNormal();
			Normals[ia] += triN;
			Normals[ib] += triN;
			Normals[ic] += triN;
		}
		for (int i = 0; i < Normals.Num(); ++i) Normals[i].Normalize();

		// Tangents (approx along theta)
		for (int theta = 0; theta <= rows; ++theta)
		{
			for (int phi = 0; phi <= cols; ++phi)
			{
				int idx = theta * (cols + 1) + phi;
				FVector before = Vertices[idx];
				FVector after = before;
				if (theta < rows)
				{
					after = Vertices[(theta + 1) * (cols + 1) + phi];
				}
				else if (theta > 0)
				{
					after = Vertices[(theta - 1) * (cols + 1) + phi];
				}
				FVector tang = (after - before).GetSafeNormal();
				Tangents[idx] = FProcMeshTangent(tang, false);
			}
		}

		// Append to All arrays with offset
		int32 localVertexCount = Vertices.Num();
		AllVertices.Append(Vertices);
		AllUVs.Append(UVs);
		AllNormals.Append(Normals);
		AllTangents.Append(Tangents);
		// Triangles need offset
		for (int id : Triangles)
		{
			AllTriangles.Add(id + vertexOffset);
		}

		vertexOffset += localVertexCount;

		// Create section for this layer by slicing the combined arrays (ProceduralMesh can't create from subset easily),
		// so instead create separate sections per layer directly - simpler and avoids extra memory copy:
		// But to keep material assignment per layer, we'll create the section here directly.
		ProcMesh->CreateMeshSection(sectionIndex, Vertices, Triangles, Normals, UVs, TArray<FColor>(), Tangents, !bDisableCollision);
		if (LayerMaterial)
		{
			ProcMesh->SetMaterial(sectionIndex, LayerMaterial);
		}
		sectionIndex++;
	}

	// NOTE: we already created individual sections inside the loop above. If you prefer to merge into a single section,
	// you can instead call CreateMeshSection once with All* arrays. We created per-layer sections to allow different materials.
}

FVector AProceduralFlower::SampleVertexForLayer(int32 LayerIndex, int32 thetaIndex, int32 phiIndex) const
{
	// Map indices to the same math as the p5 sketch
	// theta in [0..Rows] maps to theta (radius fraction)
	// phi in [0..Cols] maps to phi angle around circle

	int32 rows = FMath::Max(1, Rows);
	int32 cols = FMath::Max(3, Cols);

	float theta = float(thetaIndex); // 0..Rows
	float phi = float(phiIndex);     // 0..Cols

	float u = theta / float(rows);

	// get layer parameters
	int   pNum = LayerPetalNum.IsValidIndex(LayerIndex) ? LayerPetalNum[LayerIndex] : 5;
	float fD = LayerDiameter.IsValidIndex(LayerIndex) ? LayerDiameter[LayerIndex] : 200.f;
	float pLen = LayerPetalLength.IsValidIndex(LayerIndex) ? LayerPetalLength[LayerIndex] : 60.f;
	float pSharp = LayerPetalSharpness.IsValidIndex(LayerIndex) ? LayerPetalSharpness[LayerIndex] : 0.4f;
	float fHeight = LayerFlowerHeight.IsValidIndex(LayerIndex) ? LayerFlowerHeight[LayerIndex] : 300.f;
	float curve1 = LayerCurvature1.IsValidIndex(LayerIndex) ? LayerCurvature1[LayerIndex] : 0.8f;
	float curve2 = LayerCurvature2.IsValidIndex(LayerIndex) ? LayerCurvature2[LayerIndex] : 0.2f;
	float b = LayerBump.IsValidIndex(LayerIndex) ? LayerBump[LayerIndex] : 2.5f;
	int bNum = LayerBumpNum.IsValidIndex(LayerIndex) ? LayerBumpNum[LayerIndex] : 10;

	// angle in degrees like the p5 code:
	float angleDeg = (phi * 360.f) / float(cols);
	float sinArgDeg = (pNum / 2.f) * phi * 360.f / float(cols);
	float sinArgRad = FMath::DegreesToRadians(sinArgDeg);

	float r = (pLen * FMath::Pow(FMath::Abs(FMath::Sin(sinArgRad)), pSharp) + fD) * u; // same as JS
	float angleRad = FMath::DegreesToRadians(angleDeg);
	float x = r * FMath::Cos(angleRad);
	float y = r * FMath::Sin(angleRad);

	float z = VShape(fHeight, r / 100.f, curve1, curve2, 1.5f) - 200.f
		+ Bumpiness(b, r / 100.f, float(bNum), angleDeg);

	// Transform into world space relative to actor location
	FVector world = FVector(x, y, z) + GetActorLocation();
	return world;
}