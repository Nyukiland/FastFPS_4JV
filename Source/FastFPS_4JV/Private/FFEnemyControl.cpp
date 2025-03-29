#include "FFEnemyControl.h"

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
		UGameInstance* GameInstance = WorldContext->GetWorld()->GetGameInstance();
		if (!GameInstance) return nullptr;

		Instance = NewObject<AFFEnemyControl>(GameInstance);
		Instance->AddToRoot(); 
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
	for (int32 i = EnemiesArray.Num() - 1; i >= 0; i--)
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
	Instance = this;
}

void AFFEnemyControl::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}