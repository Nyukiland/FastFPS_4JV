// Fill out your copyright notice in the Description page of Project Settings.


#include "FFEnemyManager.h"

UFFEnemyManager* UFFEnemyManager::Instance = nullptr;

UFFEnemyManager* UFFEnemyManager::GetEnemyManager()
{
	if (Instance == nullptr)
	{
		Instance = NewObject<UFFEnemyManager>();
	}
	return Instance;
}

UFFEnemyManager::UFFEnemyManager()
{
	// Constructor 
}

UFFEnemyManager::~UFFEnemyManager()
{
	// Destructor 
}

void UFFEnemyManager::RegisterEnemySpawned(AActor* Spawned)
{
	if (Instance == nullptr) return;

	EnemiesArray.Add(Spawned);
}

void UFFEnemyManager::EnemyDied(AActor* Died)
{
	if (Instance == nullptr) return;

	if (EnemiesArray.Contains(Died))
	{
		EnemiesArray.Remove(Died);
		EnemiesKilled++;
	}
}

void UFFEnemyManager::KillAllEnemies()
{
	if (Instance == nullptr) return;

	EnemiesKilled += EnemiesArray.Num();
	EnemiesArray.Empty();
}

void UFFEnemyManager::ResetEnemyManager()
{
	if (Instance != nullptr)
	{
		// Allow the object to be garbage collected
		Instance->RemoveFromRoot();  // Unbind it from GC
		Instance = nullptr;  // Clear the Singleton instance

		// Optionally clear arrays and variables here if needed:
		EnemiesArray.Empty();
		EnemiesKilled = 0;
	}
}

TArray<AActor*> UFFEnemyManager::GetAllEnemies()
{
	if (Instance == nullptr) return TArray<AActor*>();

	return EnemiesArray;
}
