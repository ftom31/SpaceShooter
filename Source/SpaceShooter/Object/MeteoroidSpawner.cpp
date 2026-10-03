// Fill out your copyright notice in the Description page of Project Settings.

#include "Engine/World.h"
#include "MeteoroidSpawner.h"

#include "Kismet/GameplayStatics.h"

// Sets default values
AMeteoroidSpawner::AMeteoroidSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMeteoroidSpawner::BeginPlay()
{
	Super::BeginPlay();
	
	CurrentSpawnInterval = SpawnInterval;
	ScheduleNextSpawn();
}

void AMeteoroidSpawner::ScheduleNextSpawn()
{
	GetWorldTimerManager().SetTimer(
		SpawnTimerHandle, 
		this, 
		&AMeteoroidSpawner::SpawnMeteoroid, 
		CurrentSpawnInterval, 
		false
	);
}

void AMeteoroidSpawner::SpawnMeteoroid()
{
	if (!MeteoroidClass) return;

	float RandomOffset = 0.0f;
	int32 Attempts = 0;

	do
	{
		RandomOffset = FMath::RandRange(MinSpawnX, MaxSpawnX);
		Attempts++;
	} 
	while (FMath::Abs(RandomOffset - LastSpawnX) < 400.0f && Attempts < 10);

	LastSpawnX = RandomOffset;

	FVector SpawnLocation = GetActorLocation() + FVector(RandomOffset, 0.0f, 0.0f);
	SpawnLocation.Z = 2440.0f;
    
	FTransform SpawnTransform(FRotator::ZeroRotator, SpawnLocation);

	AMeteoroid* NewMeteoroid = GetWorld()->SpawnActorDeferred<AMeteoroid>(
		MeteoroidClass, 
		SpawnTransform, 
		this, 
		nullptr, 
		ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn
	);

	if (NewMeteoroid)
	{
		int32 RandomHits = FMath::RandRange(1, 8);
		NewMeteoroid->InitStats(RandomHits, FVector::ZeroVector, CurrentSpeedMultiplier);
		UGameplayStatics::FinishSpawningActor(NewMeteoroid, SpawnTransform);
	}

	CurrentSpawnInterval = FMath::Max(MinSpawnInterval, CurrentSpawnInterval - IntervalDecreaseRate);
	CurrentSpeedMultiplier = FMath::Min(MaxSpeedMultiplier, CurrentSpeedMultiplier + SpeedIncreaseRate);

	ScheduleNextSpawn();
}


// Called every frame
void AMeteoroidSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

