// Fill out your copyright notice in the Description page of Project Settings.


#include "PFWeaponBaseServer.h"
#include <Components/SkeletalMeshComponent.h>
#include <Components/SphereComponent.h>
#include "PFPlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/KismetMathLibrary.h"
#include "PFObjectPoolManager.h"
#include "Net/UnrealNetwork.h"
#include "PFGameplayFunctionLibrary.h"
#include "../Plugins/FX/Niagara/Source/Niagara/Public/NiagaraComponent.h"
#include "DrawDebugHelpers.h"

#include "PFAttributeComponent.h"

// Sets default values
APFWeaponBaseServer::APFWeaponBaseServer()
{
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	WeaponMesh->SetCollisionObjectType(ECC_WorldStatic);

	// Only visible to other players
	WeaponMesh->SetOwnerNoSee(true);
	WeaponMesh->SetOnlyOwnerSee(false);
	WeaponMesh->SetEnableGravity(true);
	WeaponMesh->SetSimulatePhysics(true);

	SphereCollision = CreateDefaultSubobject<USphereComponent>(FName("SphereCollision"));
	SphereCollision->SetupAttachment(WeaponMesh);

	//Init MuzzlePoint, which is where bullet spawn
	MuzzlePoint = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzlePoint"));
	MuzzlePoint->SetupAttachment(WeaponMesh);

	// Weapon event trigger, like pick up.
	SphereCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereCollision->SetCollisionObjectType(ECC_WorldDynamic);

	// Spawn on server and then replicate to client
	bReplicates = true;
}

void APFWeaponBaseServer::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	SphereCollision->OnComponentBeginOverlap.AddDynamic(this, &APFWeaponBaseServer::OnWeaponBeginOverlap);
}

void APFWeaponBaseServer::OnWeaponBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

}

void APFWeaponBaseServer::OnWeaponEquiped(UPrimitiveComponent* TargetPlayer, FName SocketName)
{
	WeaponMesh->SetEnableGravity(false);
	WeaponMesh->SetSimulatePhysics(false);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	SphereCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Super::OnWeaponEquiped(TargetPlayer, SocketName);
	SetEffectsOnEquiped();
}

void APFWeaponBaseServer::OnWeaponDropped()
{
	WeaponMesh->SetEnableGravity(false);
	WeaponMesh->SetSimulatePhysics(false);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	SphereCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Super::OnWeaponDropped();
	SetEffectsOnDropped();
}

// NetMulticast
void APFWeaponBaseServer::OnFireServer_Implementation(const FVector CameraLocation, const FRotator& SpawnRotation, const FVector& BulletTarget, const TArray<APFPlayerCharacter*>& TargetPlayers)
{
	if (!HasAuthority())
	{
		if (MuzzleFlash)
		{
			MuzzleFlash->Activate();
		}
		ensureMsgf(FireSound, TEXT("No Fire Sound Attached!"));
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
		return;
	}
	
	if (Ammo <= 0)
	{
		return;
	}

	LastFireTime = UGameplayStatics::GetRealTimeSeconds(GetWorld());

	if (WeaponType != EWeaponType::WT_Pistol)
		Ammo--;

	UWorld* const World = GetWorld();
	if (World)
	{
		APFPlayerCharacter* OwnerPlayer = Cast<APFPlayerCharacter>(GetOwner());
		float DamageModifier = OwnerPlayer->GetAttributeComponent()->GetPropertyModifier(EPropertyModifierType::PMT_Damage);

		//if is line trace weapon, apply damage to the target player
		if (IsLineTraceWeapon)
		{
			if (TargetPlayers.Num() != 0)
			{
				//apply damage
				
				for (int i = 0; i < TargetPlayers.Num(); i++)
				{
					UPFGameplayFunctionLibrary::ApplyDamage(OwnerPlayer, TargetPlayers[i], WeaponDamage * DamageModifier);
				}
			}
		}
		else//if is bullet weapon, create bullet based on the bullet target location
		{
			if (BulletTarget.Equals(FVector::Zero())) 
				return;
			//BulletsPerFire decides how many bullet will be spawned, only shot gun's BulletsPerFire > 1
			for (int i = 0; i < BulletsPerFire; i++)
			{
				//Spawn and Set Bullet
				if (!ObjectPoolManager)
				{
					ObjectPoolManager = Cast<APFObjectPoolManager>(UGameplayStatics::GetActorOfClass(GetWorld(), APFObjectPoolManager::StaticClass()));
				}
				APFProjectileBase* Bullet = Cast<APFProjectileBase>(ObjectPoolManager->GetObjectOfType(BulletBaseClass));

				FVector BulletDirection = BulletTarget - MuzzlePoint->GetComponentLocation();
				FVector VerticalDirection = FVector::CrossProduct(BulletDirection, FMath::VRand());
				VerticalDirection.Normalize();

				//Reset bullet direction based on Scattering angle
				BulletDirection = VerticalDirection * FVector::Distance(MuzzlePoint->GetComponentLocation(), BulletTarget) * FMath::Tan((FMath::FRandRange(-ScatteringAngle, ScatteringAngle) * 3.1415926f) / 180) + BulletDirection;

				// May failed to generate new bullet
				if (Bullet)
				{
					//Set Bullet Direction toward BulletTarget
					//BulletTarget is Line Traced by Client
					Bullet->InitLaunch(CameraLocation+UKismetMathLibrary::GetForwardVector(SpawnRotation) * 120, (BulletDirection).Rotation());
					Bullet->SetDamage(WeaponDamage * DamageModifier);
					UE_LOG(LogTemp, Warning, TEXT("Bullet Set Damage: initial %f current %f."), WeaponDamage, WeaponDamage * DamageModifier);

					if (OwnerPlayer)
					{ 
						Bullet->SetOwner(OwnerPlayer);
						Bullet->SetInstigator(OwnerPlayer);
					}
				}
			}
		}
	}
}

bool APFWeaponBaseServer::CanFire()
{
	return (UGameplayStatics::GetRealTimeSeconds(GetWorld()) - LastFireTime) >= (1 / FireRate);
}

void APFWeaponBaseServer::SetEffectsOnEquiped_Implementation()
{
	//Native implementation
}

void APFWeaponBaseServer::SetEffectsOnDropped_Implementation()
{
	//Native implementation
}
