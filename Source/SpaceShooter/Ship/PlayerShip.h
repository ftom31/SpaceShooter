// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputMappingContext.h"
#include "PaperFlipbookComponent.h"
#include "Projectile.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Pawn.h"
#include "PlayerShip.generated.h"

UCLASS()
class SPACESHOOTER_API APlayerShip : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	APlayerShip();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	virtual void PawnClientRestart() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBoxComponent* BoxComp;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = "PlayerShip")
	UPaperFlipbookComponent* FlipbookComp;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerShip")
	UPaperFlipbook* IdleAnimation;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerShip")
	UPaperFlipbook* moveAnimation;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* MoveAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* ShootAction;
	
	void Move(const FInputActionValue& Value);
	void StopMove(const FInputActionValue& Value);
	void Shoot(const FInputActionValue& Value);
	
	FVector2D MovementInput;
	
	UPROPERTY(EditAnywhere, Category = "Spawning")
	TSubclassOf<AProjectile> ItemToSpawnClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerShip")
	float MaxSpeed = 4000.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerShip")
	float Health = 3.0f;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PlayerShip")
	int32 Score = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Boundaries")
	float MinX = 1371.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Boundaries")
	float MaxX = 4703.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Boundaries")
	float MinY = -1220.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Boundaries")
	float MaxY = 628.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Boundaries")
	bool bClampY = true;

	void AddScore(int32 Amount);
	
	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
};
