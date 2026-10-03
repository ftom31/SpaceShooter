// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraSystem.h"
#include "PaperFlipbookComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "Meteoroid.generated.h"

UCLASS()
class SPACESHOOTER_API AMeteoroid : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMeteoroid();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBoxComponent* BoxComp;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = "Meteoroid")
	UPaperFlipbookComponent* FlipbookComp;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Meteoroid")
	UPaperFlipbook* Animation;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	UNiagaraSystem* ExplosionEffect;
	
	void Explode();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Meteoroid")
	float Health = 1.0f;
	
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void SplitIntoSmall();
	
	int32 InitialHits = 1;
	bool bIsInitialized = false;
	
	void InitStats(int32 InHits, FVector CustomVelocity = FVector::ZeroVector, float SpeedMultiplier = 1.0f);
};
