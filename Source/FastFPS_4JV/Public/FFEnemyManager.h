// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "FFEnemyManager.generated.h"

UCLASS()
class FASTFPS_4JV_API UFFEnemyManager : public UObject
{
	GENERATED_BODY()

private:
	UFFEnemyManager();
	~UFFEnemyManager();
	static UFFEnemyManager* Instance;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "EnemyManager")
	TArray<AActor*> EnemiesArray;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "EnemyManager")
	int EnemiesKilled;

public:

	UFUNCTION(BlueprintCallable, Category = "EnemyManager")
	static UFFEnemyManager* GetEnemyManager(UObject* WorldContext);

	UFUNCTION(BlueprintCallable, Category = "EnemyManager")
	void RegisterEnemySpawned(AActor* Spawned);

	UFUNCTION(BlueprintCallable, Category = "EnemyManager")
	void EnemyDied(AActor* Died);

	UFUNCTION(BlueprintCallable, Category = "EnemyManager")
	void KillAllEnemies();

	UFUNCTION(BlueprintCallable, Category = "EnemyManager")
	void ResetEnemyManager();

	UFUNCTION(BlueprintCallable, Category = "EnemyManager")
	TArray<AActor*> GetAllEnemies();

protected:
	virtual void BeginDestroy() override;
};