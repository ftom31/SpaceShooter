// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"

#include "Kismet/GameplayStatics.h"

// Sets default values
AProjectile::AProjectile()
{
	PrimaryActorTick.bCanEverTick = false;
	
	BoxComp = CreateDefaultSubobject<UBoxComponent>(FName("BoxComponent"));
	RootComponent = BoxComp;
    
	BoxComp->InitBoxExtent(FVector(10.0f, 10.0f, 10.0f));
    
	BoxComp->SetSimulatePhysics(true);
	BoxComp->SetEnableGravity(false);
	BoxComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	BoxComp->BodyInstance.bLockZTranslation = true;
	BoxComp->BodyInstance.bLockXRotation = true;
	BoxComp->BodyInstance.bLockYRotation = true;
	BoxComp->BodyInstance.bLockZRotation = true;
    
	BoxComp->SetGenerateOverlapEvents(true);
	BoxComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	BoxComp->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Overlap);
	BoxComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);       
	
	BoxComp->OnComponentBeginOverlap.AddDynamic(this, &AProjectile::OnOverlap);
    
	FlipbookComp = CreateDefaultSubobject<UPaperFlipbookComponent>(FName("Flipbook"));
	FlipbookComp->SetupAttachment(RootComponent);
	FlipbookComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    
	InitialLifeSpan = 3.0f;
}

void AProjectile::OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != this && OtherActor != GetOwner())
	{
		UGameplayStatics::ApplyDamage(OtherActor, Damage, GetInstigatorController(), this, UDamageType::StaticClass());
		Destroy();
	}
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	BoxComp->SetPhysicsLinearVelocity(GetActorForwardVector() * 2000.0f);
}

// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

