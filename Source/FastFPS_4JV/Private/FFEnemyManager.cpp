// Fill out your copyright notice in the Description page of Project Settings.


#include "FFEnemyManager.h"

UFFEnemyManager* UFFEnemyManager::Instance = nullptr;

UFFEnemyManager::UFFEnemyManager() : EnemiesKilled(0) {}

UFFEnemyManager::~UFFEnemyManager() {}

UFFEnemyManager* UFFEnemyManager::GetEnemyManager(UObject* WorldContext)
{
	if (!Instance)
	{
		UGameInstance* GameInstance = WorldContext->GetWorld()->GetGameInstance();
		if (!GameInstance) return nullptr;

		Instance = NewObject<UFFEnemyManager>(GameInstance);
		Instance->AddToRoot();  // Prevent GC
	}
	return Instance;
}

void UFFEnemyManager::RegisterEnemySpawned(AActor* Spawned)
{
	if (!Instance) return;
	EnemiesArray.Add(Spawned);
}

void UFFEnemyManager::EnemyDied(AActor* Died)
{
	if (!Instance) return;

	if (EnemiesArray.Contains(Died))
	{
		EnemiesArray.Remove(Died);
		EnemiesKilled++;
	}
}

void UFFEnemyManager::KillAllEnemies()
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

void UFFEnemyManager::ResetEnemyManager()
{
	if (Instance)
	{
		Instance->RemoveFromRoot();
		Instance = nullptr;
	}
}

TArray<AActor*> UFFEnemyManager::GetAllEnemies()
{
	return Instance ? EnemiesArray : TArray<AActor*>();
}

void UFFEnemyManager::BeginDestroy()
{
	Super::BeginDestroy();
	// Optional cleanup
}