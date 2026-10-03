// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperFlipbook.h"
#include "PaperFlipbookComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

UCLASS()
class SPACESHOOTER_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBoxComponent* BoxComp;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = "Proctile")
	UPaperFlipbookComponent* FlipbookComp;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Proctile")
	UPaperFlipbook* Animation;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Proctile")
	float Speed = 500.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Proctile")
	float Damage = 1;
	
	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
