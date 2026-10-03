// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Meteoroid.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "MeteoroidSpawner.generated.h"

UCLASS()
class SPACESHOOTER_API AMeteoroidSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMeteoroidSpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category = "Spawning")
	TSubclassOf<AMeteoroid> MeteoroidClass;

	UPROPERTY(EditAnywhere, Category = "Spawning")
	float SpawnInterval = 1.5f;

	UPROPERTY(EditAnywhere, Category = "Spawning")
	float MinSpawnX = -1000.0f;

	UPROPERTY(EditAnywhere, Category = "Spawning")
	float MaxSpawnX = 2000.0f;

	UPROPERTY(EditAnywhere, Category = "Spawning")
	float SpawnTopCoordinate = 2000.0f;
	
	float LastSpawnX = -99999.0f;

	FTimerHandle SpawnTimerHandle;

	void SpawnMeteoroid();
	
	
	float CurrentSpawnInterval;

	UPROPERTY(EditAnywhere, Category = "Spawning")
	float MinSpawnInterval = 0.35f;

	UPROPERTY(EditAnywhere, Category = "Spawning")
	float IntervalDecreaseRate = 0.02f;

	void ScheduleNextSpawn();
	
	float CurrentSpeedMultiplier = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Spawning")
	float MaxSpeedMultiplier = 2.2f;

	UPROPERTY(EditAnywhere, Category = "Spawning")
	float SpeedIncreaseRate = 0.015f;
};
