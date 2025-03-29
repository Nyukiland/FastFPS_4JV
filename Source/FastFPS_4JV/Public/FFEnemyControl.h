#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FFEnemyControl.generated.h"

//USTRUCT(BlueprintType)
//struct WaveSetUp
//{
//	GENERATED_BODY()
//
//public:
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EnemyManager")
//	int EnemyCount;
//
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EnemyManager")
//	int EnemyAtTheSameTime;
//
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EnemyManager")
//	float TimeSpawn;
//};

UCLASS()
class FASTFPS_4JV_API AFFEnemyControl : public AActor
{
	GENERATED_BODY()
	
public:	
	AFFEnemyControl();


public:
	static AFFEnemyControl* Instance;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "EnemyManager")
	TArray<AActor*> EnemiesArray;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "EnemyManager")
	int EnemiesKilled;

public:
	UFUNCTION(BlueprintCallable, Category = "EnemyManager")
	static AFFEnemyControl* GetEnemyControl(UObject* WorldContext);

	UFUNCTION(BlueprintCallable, Category = "EnemyManager")
	void RegisterEnemySpawned(AActor* Spawned);

	UFUNCTION(BlueprintCallable, Category = "EnemyManager")
	void EnemyDied(AActor* Died);

	UFUNCTION(BlueprintCallable, Category = "EnemyManager")
	void KillAllEnemies();

	UFUNCTION(BlueprintCallable, Category = "EnemyManager")
	TArray<AActor*> GetAllEnemies();

protected:	
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
};
