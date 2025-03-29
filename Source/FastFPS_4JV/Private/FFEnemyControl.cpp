#include "FFEnemyControl.h"
#include "EngineUtils.h" 

AFFEnemyControl* AFFEnemyControl::Instance = nullptr;

AFFEnemyControl::AFFEnemyControl()
{
	PrimaryActorTick.bCanEverTick = true;

	EnemiesKilled = 0;
}

AFFEnemyControl* AFFEnemyControl::GetEnemyControl(UObject* WorldContext)
{
	if (!Instance)
	{
		UWorld* World = WorldContext ? WorldContext->GetWorld() : nullptr;
		if (!World) return nullptr;

		for (TActorIterator<AFFEnemyControl> It(World); It; ++It)
		{
			if (*It)
			{
				Instance = *It;
				break;
			}
		}

		if (!Instance)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.bNoFail = true;
			Instance = World->SpawnActor<AFFEnemyControl>(AFFEnemyControl::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
		}
	}
	return Instance;
}

void AFFEnemyControl::RegisterEnemySpawned(AActor* Spawned)
{
	if (!Instance) return;
	EnemiesArray.Add(Spawned);
}

void AFFEnemyControl::EnemyDied(AActor* Died)
{
	if (!Instance) return;

	if (EnemiesArray.Contains(Died))
	{
		EnemiesArray.Remove(Died);
		EnemiesKilled++;
	}
}

void AFFEnemyControl::KillAllEnemies()
{
	if (!Instance) return;

	EnemiesKilled += EnemiesArray.Num();
	for (int i = EnemiesArray.Num() - 1; i >= 0; i--)
	{
		if (EnemiesArray[i])
		{
			EnemiesArray[i]->Destroy();
		}
	}
	EnemiesArray.Empty();
}

TArray<AActor*> AFFEnemyControl::GetAllEnemies()
{
	return Instance ? EnemiesArray : TArray<AActor*>();
}

void AFFEnemyControl::BeginPlay()
{
	Super::BeginPlay();
	if (!Instance) Instance = this;
	else this->Destroy();
}

void AFFEnemyControl::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AFFEnemyControl::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (Instance == this) Instance = nullptr;
}