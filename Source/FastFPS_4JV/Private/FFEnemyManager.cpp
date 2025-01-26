// Fill out your copyright notice in the Description page of Project Settings.


#include "FFEnemyManager.h"

UFFEnemyManager* UFFEnemyManager::Instance = nullptr;

UFFEnemyManager* UFFEnemyManager::GetEnemyManager()
{
	if (Instance == nullptr)
	{
		Instance = NewObject<UFFEnemyManager>();
		Instance->AddToRoot(); // Prevent garbage collection
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
	//EnemiesArray.Add(Spawned);
}

void UFFEnemyManager::EnemyDied(AActor* Died)
{
	/*if (EnemiesArray.Contains(Died))
	{
		EnemiesArray.Remove(Died);
		EnemiesKilled++;
	}*/
}

void UFFEnemyManager::KillAllEnemies()
{
	/*EnemiesKilled += EnemiesArray.Num();
	EnemiesArray.Empty();*/
}

TArray<AActor*> UFFEnemyManager::GetAllEnemies()
{
	return EnemiesArray;
}
