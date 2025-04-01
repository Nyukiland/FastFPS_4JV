#include "FFEnemyControl.h"
#include "EngineUtils.h" 

AFFEnemyControl* AFFEnemyControl::Instance = nullptr;

AFFEnemyControl::AFFEnemyControl()
{
	PrimaryActorTick.bCanEverTick = true;

	EnemiesKilled = 0;
	CurrentWave = 0;
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

void AFFEnemyControl::RegisterPlayer(AActor* CurPlayer)
{
	Player = CurPlayer;
}

void AFFEnemyControl::RegisterEnemySpawned(AActor* Spawned)
{
	if (!Instance) return;
	EnemiesArray.Add(Spawned);
	if (!Spawned->GetClass()->IsChildOf(SpawnerClass)) EnemySpawned++;
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

FWaveSetUp AFFEnemyControl::GetCurrentWaveInfo()
{
	if (CurrentWave >= Waves.Num()) return FWaveSetUp();
	return Waves[CurrentWave];
}

void AFFEnemyControl::BeginPlay()
{
	Super::BeginPlay();
	if (!Instance) Instance = this;
	else
	{
		this->Destroy();
		return;
	}

	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &AFFEnemyControl::DelayedBegin);
}

void AFFEnemyControl::DelayedBegin()
{
	if (Waves.Num() == 0) return;

	for (int i = EnemiesArray.Num() - 1; i >= 0; --i)
	{
		if (EnemiesArray[i] && EnemiesArray[i]->GetClass()->IsChildOf(SpawnerClass))
		{
			SpawnerPos.Add(EnemiesArray[i]->GetActorLocation());

			AActor* EnemyToDestroy = EnemiesArray[i];
			EnemiesArray.RemoveAt(i);

			if (EnemyToDestroy) EnemyToDestroy->Destroy();
		}
	}

	PlaceSpawner(0);
}

void AFFEnemyControl::PlaceSpawner(int WaveCount)
{
	if (!Player) return;

	SpawnerPos.Sort([this](const FVector& A, const FVector& B)
		{
			FVector PlayerPos = Player->GetActorLocation();
			return FVector::Dist(A, PlayerPos) > FVector::Dist(B, PlayerPos);
		});

	for (int i = 0; i < Waves[WaveCount].SpawnerCount; i++)
	{
		AActor* Spawner = GetWorld()->SpawnActor<AActor>(SpawnerClass);
		Spawner->SetActorLocation(SpawnerPos[FMath::RandRange(i, SpawnerPos.Num() - 2)]);
	}
}

void AFFEnemyControl::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Waves.Num() == 0) return;

	WaveControlPassive(DeltaTime);
	WaveControlAttack(DeltaTime);
}

void AFFEnemyControl::WaveControlPassive(float DeltaTime)
{
	if (WaveState != EWAVESTATE::InBetween) return;

	Timer += DeltaTime;

	if (Timer > TimeBetweenWave)
	{
		WaveState = EWAVESTATE::InWave;
		Timer = 0;

		PlaceSpawner(CurrentWave);
	}
}

void AFFEnemyControl::WaveControlAttack(float DeltaTime)
{
	if (WaveState != EWAVESTATE::InWave) return;

	Timer += DeltaTime;

	if (Timer < 0.2f) return; //wait a little for enemies to spawn

	if (EnemiesArray.IsEmpty())
	{
		CurrentWave++;
		if (CurrentWave >= Waves.Num())
		{
			WaveState = EWAVESTATE::Win;
			return;
		}

		EnemySpawned = 0;

		TimerEachWave.Add(Timer);
		Timer = 0;
		WaveState = EWAVESTATE::InBetween;
	}
}

void AFFEnemyControl::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (Instance == this) Instance = nullptr;
}