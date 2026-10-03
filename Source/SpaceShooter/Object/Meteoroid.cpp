// Fill out your copyright notice in the Description page of Project Settings.


#include "Meteoroid.h"

#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "SpaceShooter/Ship/PlayerShip.h"

// Sets default values
AMeteoroid::AMeteoroid()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	BoxComp = CreateDefaultSubobject<UBoxComponent>(FName("BoxComponent"));
	RootComponent = BoxComp;
	
	BoxComp->SetCollisionProfileName(TEXT("PhysicsActor"));
	BoxComp->SetGenerateOverlapEvents(true);
	
	BoxComp->InitBoxExtent(FVector(20.0f, 20.0f, 50.0f));
	BoxComp->SetSimulatePhysics(true);
	BoxComp->SetEnableGravity(false);
	BoxComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	
	BoxComp->SetGenerateOverlapEvents(true);
	BoxComp->SetCollisionObjectType(ECC_PhysicsBody);
	BoxComp->SetCollisionResponseToAllChannels(ECR_Block);
	BoxComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	
	BoxComp->SetNotifyRigidBodyCollision(true);
	
	BoxComp->OnComponentBeginOverlap.AddDynamic(this, &AMeteoroid::OnOverlap);
	
	BoxComp->BodyInstance.bLockZTranslation = true;
	BoxComp->BodyInstance.bLockXRotation = true;
	BoxComp->BodyInstance.bLockYRotation = true;
	BoxComp->BodyInstance.bLockZRotation = true;
	
	InitialLifeSpan = 10.0f;
	
	FlipbookComp = CreateDefaultSubobject<UPaperFlipbookComponent>(FName("Flipbook"));
	FlipbookComp->SetupAttachment(RootComponent);
	FlipbookComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// Called when the game starts or when spawned
void AMeteoroid::BeginPlay()
{
	Super::BeginPlay();
	
	if (!bIsInitialized)
	{
		int32 RandomHits = FMath::RandRange(1, 8);
		InitStats(RandomHits);
	}
}

void AMeteoroid::InitStats(int32 InHits, FVector CustomVelocity, float SpeedMultiplier)
{
	bIsInitialized = true;
	InitialHits = InHits;
	Health = static_cast<float>(InitialHits);

	float ScaleFactor;
	float FallSpeed;

	if (InitialHits < 3) 
	{
		ScaleFactor = 0.5f;
		FallSpeed = 380.0f;
	}
	else if (InitialHits <= 5) 
	{
		ScaleFactor = 0.9f;
		FallSpeed = 300.0f;
	}
	else
	{
		ScaleFactor = 1.3f;
		FallSpeed = 220.0f;
	}
	FallSpeed *= SpeedMultiplier;
	float BaseSize = 200.0f;

	BoxComp->SetSimulatePhysics(false);
	BoxComp->SetBoxExtent(FVector(BaseSize * ScaleFactor, BaseSize * ScaleFactor, 50.0f), true);
	FlipbookComp->SetRelativeScale3D(FVector(ScaleFactor, ScaleFactor, ScaleFactor));

	BoxComp->SetLinearDamping(0.0f);
	BoxComp->SetEnableGravity(false);
	BoxComp->SetSimulatePhysics(true);

	if (!CustomVelocity.IsZero())
	{
		BoxComp->SetPhysicsLinearVelocity(CustomVelocity);
	}
	else
	{
		FVector FallDirection = FVector(0.0f, -1.0f, 0.0f);
		BoxComp->SetPhysicsLinearVelocity(FallDirection * FallSpeed);
	}
}

void AMeteoroid::SplitIntoSmall()
{
	UWorld* World = GetWorld();
	if (!World) return;

	FVector Offsets[2] = { FVector(-120.0f, 0.0f, 0.0f), FVector(120.0f, 0.0f, 0.0f) };
    
	FVector SplitVelocities[2] = { 
		FVector(-250.0f, -300.0f, 0.0f), 
		FVector(250.0f, -300.0f, 0.0f)  
	 };

	FVector CurrentLocation = GetActorLocation();

	for (int32 i = 0; i < 2; i++)
	{
		FVector SpawnLoc = CurrentLocation + Offsets[i];
		SpawnLoc.Z = CurrentLocation.Z; 

		FTransform SpawnTransform(FRotator::ZeroRotator, SpawnLoc);

		AMeteoroid* NewMeteoroid = World->SpawnActorDeferred<AMeteoroid>(
			GetClass(), 
			SpawnTransform, 
			nullptr, 
			nullptr, 
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn
		);

		if (NewMeteoroid)
		{
			NewMeteoroid->InitStats(1, SplitVelocities[i]);

			UGameplayStatics::FinishSpawningActor(NewMeteoroid, SpawnTransform);

			if (NewMeteoroid->BoxComp)
			{
				NewMeteoroid->BoxComp->SetPhysicsLinearVelocity(SplitVelocities[i]);
			}
		}
	}
}


float AMeteoroid::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	Health -= ActualDamage;

	if (Health <= 0.0f)
	{
		APlayerShip* Player = Cast<APlayerShip>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
		if (Player)
		{
			int32 Points = (InitialHits > 5) ? 250 : (InitialHits >= 3 ? 100 : 50);
			Player->AddScore(Points);
		}
		if (InitialHits > 5)
		{
			SplitIntoSmall();
		}
		Explode();
	}

	return ActualDamage;
}


void AMeteoroid::OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != this && OtherActor != GetOwner())
	{
		if (OtherActor->IsA(APlayerShip::StaticClass()))
		{
			UGameplayStatics::ApplyDamage(OtherActor, 1.0f, nullptr, this, UDamageType::StaticClass());
			Explode();
		}
	}
}

// Called every frame
void AMeteoroid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMeteoroid::Explode()
{
	if (ExplosionEffect)
	{
		FVector SpawnPos = GetActorLocation();
		SpawnPos.Z = FlipbookComp->GetComponentLocation().Z;
		
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ExplosionEffect, SpawnPos, FRotator::ZeroRotator, FVector(2.0f));
	}

	Destroy();
}

