// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerShip.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/GameplayStatics.h"
#include "SpaceShooter/Object/Meteoroid.h"

// Sets default values
APlayerShip::APlayerShip()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	AutoPossessPlayer = EAutoReceiveInput::Player0;
	
	BoxComp = CreateDefaultSubobject<UBoxComponent>(FName("Box"));
	RootComponent = BoxComp;
	BoxComp->SetCollisionProfileName(TEXT("Pawn"));
	
	BoxComp->InitBoxExtent(FVector(32.0f, 32.0f, 10.0f));
	BoxComp->SetSimulatePhysics(true);
	BoxComp->SetEnableGravity(false);
	BoxComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	BoxComp->SetGenerateOverlapEvents(true);
	BoxComp->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Overlap);
	
	BoxComp->BodyInstance.bLockZTranslation = true;
	BoxComp->BodyInstance.bLockXRotation = true;
	BoxComp->BodyInstance.bLockYRotation = true;
	BoxComp->BodyInstance.bLockZRotation = true;
	
	BoxComp->OnComponentBeginOverlap.AddDynamic(this, &APlayerShip::OnOverlap);
	
	FlipbookComp = CreateDefaultSubobject<UPaperFlipbookComponent>(FName("Flipbook"));
	FlipbookComp->SetupAttachment(RootComponent);

}

void APlayerShip::OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != this && OtherActor != GetOwner())
	{
		UGameplayStatics::ApplyDamage(this, 1.0f, nullptr, OtherActor, UDamageType::StaticClass());
		if (AMeteoroid* Met = Cast<AMeteoroid>(OtherActor))
		{
			Met->Explode();
		}
		else
		{
			OtherActor->Destroy();
		}
	}
}


float APlayerShip::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	Health -= ActualDamage;

	if (Health <= 0.0f)
	{
		APlayerController* PC = Cast<APlayerController>(GetController());
		if (PC)
		{
			DisableInput(PC);
			PC->bShowMouseCursor = true;
			PC->SetInputMode(FInputModeGameAndUI());
		}

		MovementInput = FVector2D::ZeroVector;
    
		if (BoxComp)
		{
			BoxComp->SetPhysicsLinearVelocity(FVector::ZeroVector);
			BoxComp->SetSimulatePhysics(false); 
		}

		SetActorEnableCollision(false);
		SetActorHiddenInGame(true);
	}

	return ActualDamage;
}


// Called when the game starts or when spawned
void APlayerShip::BeginPlay()
{
	Super::BeginPlay();
	
	BoxComp->SetLinearDamping(4.0f);
	BoxComp->SetAngularDamping(5.0f);
	
	if (IdleAnimation)
	{
		FlipbookComp->SetFlipbook(IdleAnimation);
	}
}

// Called every frame
void APlayerShip::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
    
	if (Health <= 0.0f)
	{
		return;
	}  
	if (!MovementInput.IsNearlyZero())
	{
		FVector ForceDirection = FVector(-MovementInput.X, MovementInput.Y, 0.0f).GetSafeNormal();

		FVector ForceToApply = ForceDirection * MaxSpeed;
		BoxComp->AddForce(ForceToApply, NAME_None, true);
	}
    
	FVector ClampedLocation = GetActorLocation();
	FVector CurrentVelocity = BoxComp->GetPhysicsLinearVelocity();

	if (ClampedLocation.X <= MinX)
	{
		ClampedLocation.X = MinX;
		if (CurrentVelocity.X < 0.0f) CurrentVelocity.X = 0.0f;
	}
	else if (ClampedLocation.X >= MaxX)
	{
		ClampedLocation.X = MaxX;
		if (CurrentVelocity.X > 0.0f) CurrentVelocity.X = 0.0f; 
	}

	if (bClampY)
	{
		if (ClampedLocation.Y <= MinY)
		{
			ClampedLocation.Y = MinY;
			if (CurrentVelocity.Y < 0.0f) CurrentVelocity.Y = 0.0f;
		}
		else if (ClampedLocation.Y >= MaxY)
		{
			ClampedLocation.Y = MaxY;
			if (CurrentVelocity.Y > 0.0f) CurrentVelocity.Y = 0.0f;
		}
	}

	SetActorLocation(ClampedLocation);
	BoxComp->SetPhysicsLinearVelocity(CurrentVelocity);
}

void APlayerShip::AddScore(int32 Amount)
{
	Score += Amount;
}


// Called to bind functionality to input
void APlayerShip::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerShip::Move);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &APlayerShip::StopMove);
		EnhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Started, this, &APlayerShip::Shoot);
	}
}


void APlayerShip::Move(const FInputActionValue& Value)
{
	MovementInput = Value.Get<FVector2D>();
}

void APlayerShip::StopMove(const FInputActionValue& Value)
{
	MovementInput = FVector2D::ZeroVector;
}


void APlayerShip::Shoot(const FInputActionValue& Value)
{
	if (ItemToSpawnClass)
	{
		FVector ForwardDir = FVector(0.0f, 1.0f, 0.0f); 

		FVector SpawnLocation = GetActorLocation() + (ForwardDir * 60.0f);
		
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = GetInstigator();
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		AProjectile* Proj = GetWorld()->SpawnActor<AProjectile>(ItemToSpawnClass, SpawnLocation, ForwardDir.Rotation(), SpawnParams);
	}
}


void APlayerShip::PawnClientRestart()
{
	Super::PawnClientRestart();

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			if (DefaultMappingContext)
			{
				Subsystem->ClearAllMappings();
				Subsystem->AddMappingContext(DefaultMappingContext, 0);
			}
		}
	}
}