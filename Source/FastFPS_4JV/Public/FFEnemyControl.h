#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FFEnemyControl.generated.h"

USTRUCT(BlueprintType)
struct FWaveSetUp
{
	GENERATED_BODY()

public:
	FWaveSetUp()
	{
		EnemyCount = 0;
		FlyingOnSpawner = 0;
		SpawnerCount = 0;
		EnemyAtTheSameTime = 0;
		TimeSpawn = 0;
		TimeAfterWave = 2;
	}

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EnemyManager")
	int EnemyCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EnemyManager")
	int FlyingOnSpawner;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EnemyManager")
	int SpawnerCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EnemyManager")
	int EnemyAtTheSameTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EnemyManager")
	float TimeSpawn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EnemyManager")
	float TimeAfterWave;
};

UENUM(BlueprintType)
enum class EWAVESTATE : uint8
{
	InWave,
	InBetween,
	Win,
	None
};

UCLASS()
class FASTFPS_4JV_API AFFEnemyControl : public AActor
{
	GENERATED_BODY()
	
public:	
	AFFEnemyControl();


public:
	static AFFEnemyControl* Instance;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "EnemyManagerDebug")
	AActor* Player;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "EnemyManagerDebug")
	TArray<AActor*> EnemiesArray;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "EnemyManagerDebug")
	int EnemiesKilled;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "EnemyManagerDebug")
	float Timer;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "EnemyManagerDebug")
	EWAVESTATE WaveState;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "EnemyManagerDebug")
	TArray<float> TimerEachWave;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EnemyManager")
	TArray<FWaveSetUp> Waves;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "EnemyManagerDebug")
	TArray<FVector> SpawnerPos;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "EnemyManagerDebug")
	int CurrentWave;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "EnemyManagerDebug")
	int EnemySpawned;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EnemyManager")
	TSubclassOf<AActor> SpawnerClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EnemyManager")
	TSubclassOf<AActor> CombinedClass;

public:
	UFUNCTION(BlueprintCallable, Category = "EnemyManager")
	static AFFEnemyControl* GetEnemyControl(UObject* WorldContext);

	UFUNCTION(BlueprintCallable, Category = "EnemyManager")
	void RegisterPlayer(AActor* CurPlayer);

	UFUNCTION(BlueprintCallable, Category = "EnemyManager")
	void RegisterEnemySpawned(AActor* Spawned);

	UFUNCTION(BlueprintCallable, Category = "EnemyManager")
	void EnemyDied(AActor* Died);

	UFUNCTION(BlueprintCallable, Category = "EnemyManager")
	void KillAllEnemies();

	UFUNCTION(BlueprintCallable, Category = "EnemyManager")
	TArray<AActor*> GetAllEnemies();

	UFUNCTION(BlueprintCallable, Category = "EnemyManager")
	FWaveSetUp GetCurrentWaveInfo();

protected:	
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void DelayedBegin();
	void PlaceSpawner(int WaveCount);
	void WaveControlPassive(float DeltaTime);
	void WaveControlAttack(float DeltaTime);
};
