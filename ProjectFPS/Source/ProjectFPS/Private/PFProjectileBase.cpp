// Fill out your copyright notice in the Description page of Project Settings.


#include "PFProjectileBase.h"
#include <Components/SphereComponent.h>
#include <GameFramework/ProjectileMovementComponent.h>
#include <Components/StaticMeshComponent.h>
#include "PFPlayerCharacter.h"
#include <Kismet/GameplayStatics.h>
#include "PFGameplayFunctionLibrary.h"
#include "PFObjectPoolManager.h"

// Sets default values
APFProjectileBase::APFProjectileBase()
{
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);

	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	RootComponent = CollisionComp;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	// ProjectileMovement->InitialSpeed = 3000.0f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(CollisionComp);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	bReplicates = true;
}

void APFProjectileBase::FellOutOfWorld(const class UDamageType& dmgType)
{
	// Do Not Destroy!
	ReturnToPool();
}

void APFProjectileBase::OutsideWorldBounds()
{
	// Do Not Destroy!
	ReturnToPool();
}

void APFProjectileBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	CollisionComp->OnComponentHit.AddDynamic(this, &APFProjectileBase::OnHit);		// set up a notification for when this component hits something blocking

	OnDestroyed.AddDynamic(this, &APFProjectileBase::OnDestroyedCallback);

	ProjectileMovement->bSimulationEnabled = false;
}

void APFProjectileBase::OnActive()
{
	FTimerDelegate TimerDelegate;

	//Binding our Lambda expression
	TimerDelegate.BindLambda([&]()
		{
			Explode();
		});

	GetWorld()->GetTimerManager().SetTimer(FlyingTimerHandle, TimerDelegate, MaxLifeTime, false);
}

void APFProjectileBase::InitLaunch_Implementation(const FVector& NewLocation, FRotator NewRotation)
{
	OnActive();

	SetActorLocation(NewLocation);
	SetActorRotation(NewRotation);

	ProjectileMovement->SetVelocityInLocalSpace(LaunchSpeed);
	ProjectileMovement->bSimulationEnabled = true;
	CollisionComp->SetCollisionProfileName("Projectile");

	IsInUse = true;
}

void APFProjectileBase::OnDestroyedCallback(AActor* Actor)
{
	// fix error: prevent bullet destroyed but FlyingTimerHandle still activate
	if (GetWorldTimerManager().IsTimerActive(FlyingTimerHandle))
		GetWorldTimerManager().ClearTimer(FlyingTimerHandle);

	FString NetmodeStr =  UPFGameplayFunctionLibrary::ConvertNetModeEnumToString(GetNetMode());
	UE_LOG(LogTemp, Warning, TEXT("On Bullet Destroyed %s, Net Mode %s!"), *GetName(), *NetmodeStr);
}

void APFProjectileBase::OnDeActive()
{
	//ProjectileMovement->SetVelocityInLocalSpace(FVector::ZeroVector);
	SetActorLocation(FVector(0.0f, 0.0f, 0.0f));
	ProjectileMovement->bSimulationEnabled = false;
	CollisionComp->SetCollisionProfileName("IgnoreAll");

	GetWorld()->GetTimerManager().ClearTimer(FlyingTimerHandle);
	IsInUse = false;
}

bool APFProjectileBase::IsBussy()
{
	return IsInUse;
}

void APFProjectileBase::SetObjectPoolManager(class APFObjectPoolManager* ObjectManager)
{
	this->ObjectPoolManager = ObjectManager;
}

void APFProjectileBase::Explode_Implementation()
{
	ReturnToPool();
}

void APFProjectileBase::ReturnToPool_Implementation()
{
	UWorld* World = GetWorld();
	World->GetTimerManager().ClearTimer(FlyingTimerHandle);

	if(HasAuthority())
	{ 
		ensureMsgf(ObjectPoolManager, TEXT("Associated Object Pool Manager is nullptr!"));
		ObjectPoolManager->ReturnToPool(this);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("On Bullet Return To Pool %s!"), *GetName());
		OnDeActive();
	}
}

void APFProjectileBase::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	APFPlayerCharacter* OtherPlayerCharacter = Cast<APFPlayerCharacter>(OtherActor);
	APFPlayerCharacter* InstigatorCharacter = GetInstigator() ? Cast<APFPlayerCharacter>(GetInstigator()) : nullptr;

	if (OtherPlayerCharacter && InstigatorCharacter && OtherPlayerCharacter->GetTeamID() == InstigatorCharacter->GetTeamID())
		return;

	if (HasAuthority())
	{
		UPFGameplayFunctionLibrary::ApplyDamage(InstigatorCharacter, OtherActor, DamageAmount);
	}

	Explode();
}

void APFProjectileBase::SetDamage(const float Damage)
{
	DamageAmount = Damage;
}