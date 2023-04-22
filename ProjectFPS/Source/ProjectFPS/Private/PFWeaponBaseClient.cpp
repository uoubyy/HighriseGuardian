// Copyright FPS Team. All Rights Reserved.


#include "PFWeaponBaseClient.h"
#include "PFWeaponBaseServer.h"
#include "../Public/NiagaraComponent.h"
#include "Camera/CameraActor.h"
#include "../Public/NiagaraFunctionLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "PFPlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

APFWeaponBaseClient::APFWeaponBaseClient()
{

}

void APFWeaponBaseClient::OnWeaponEquiped(UPrimitiveComponent* TargetPlayer, FName SocketName)
{
	Super::OnWeaponEquiped(TargetPlayer, SocketName);
	SetEffectsOnEquiped();

	USkeletalMeshComponent* OwnerSkeletonMesh = Cast<USkeletalMeshComponent>(TargetPlayer);
	if (OwnerSkeletonMesh)
	{
		OwnerSkeletonMesh->LinkAnimClassLayers(WeaponEquippedAnimClass);
	}
}

void APFWeaponBaseClient::OnWeaponDropped()
{
	Super::OnWeaponDropped();
	SetEffectsOnDropped();
}

bool APFWeaponBaseClient::CanFire()
{
	return Ammo > 0 && WeaponStatus == EWeaponStatusType::WST_Equipped && (UGameplayStatics::GetRealTimeSeconds(GetWorld()) - LastFireTime) >= (1 / FireRate);
}

void APFWeaponBaseClient::SyncWeaponProperties(APFWeaponBaseServer* WeaponServer)
{
	// TODO: disable the weapon invalidation check temporarily
	if (!WeaponServer || WeaponServer->GetWeaponType() != GetWeaponType())
	{
		UE_LOG(LogTemp, Warning, TEXT("Weapon Client Sync Weapon Properties with invalid weapon server "));
		return;
	}
	// ensureMsgf(WeaponServer && WeaponServer->GetWeaponType() == GetWeaponType(), TEXT("Weapon Client Sync Weapon Properties with invalid weapon server"));

	if(WeaponServer)
	{ 
		Ammo = WeaponServer->GetCurrAmmo();
		MaxAmmo = WeaponServer->GetMaxAmmo();
		IsAuto = WeaponServer->IsAutoWeapon();
		IsLineTraceWeapon = WeaponServer->IsLineTraceShoot();
		WeaponDamage = WeaponServer->GetWeaponDamage();
		FireRate = WeaponServer->GetFireRate();
		LastFireTime = WeaponServer->GetLastFireTime();
		BulletsPerFire = WeaponServer->GetBulletsPerFire();
		ScatteringAngle = WeaponServer->GetScatteringAngle();
	}
}

bool APFWeaponBaseClient::EquipScopeCamera(class ACameraActor* ScopeCamera)
{
	if (!ScopeCameraEnabled)
		return false;

	ScopeCamera->K2_AttachToComponent(GetMesh(), ScopeCameraSocketName, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, true);
	return true;
}

void APFWeaponBaseClient::OnFireClient(const FVector CameraLocation, const FRotator SpawnRotation, class APFPlayerController* PlayerController, FVector& BulletTarget, TArray<APFPlayerCharacter*>& TargetPlayers)
{
	if (Ammo <= 0)
		return;

	OnFireBP();

	LastFireTime = UGameplayStatics::GetRealTimeSeconds(GetWorld());

	if(WeaponType!=EWeaponType::WT_Pistol)
		Ammo--;

	//Activate muzzle flash
	if (MuzzleFlash)
	{
		MuzzleFlash->Activate();
	}
	//ensureMsgf(FireSound, TEXT("No Fire Sound Attached!"));
	//UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());

	//Line track
	UWorld* World = GetWorld();
	if (World)
	{
		if (IsLineTraceWeapon)
		{
			APFPlayerCharacter* OwnerPlayer = Cast<APFPlayerCharacter>(GetOwner());
			int32 MyTeamID = OwnerPlayer->GetTeamID();

			for (int i = 0; i < BulletsPerFire; i++)
			{
				TArray<AActor*> IgnoreArray;
				IgnoreArray.Add(this);
				IgnoreArray.Add(GetOwner());

				FHitResult HitResult;
				FVector EndLocation = CameraLocation + UKismetMathLibrary::GetForwardVector(SpawnRotation) * 10000;
				{
					bool HitSuccess = UKismetSystemLibrary::LineTraceSingle(
						World, CameraLocation, EndLocation, ETraceTypeQuery::TraceTypeQuery1, false,
						IgnoreArray, EDrawDebugTrace::None
						, HitResult, true);

					if (HitSuccess)
						BulletTarget = HitResult.ImpactPoint;
					else
						BulletTarget = EndLocation;
				}

				FVector BulletDirection = BulletTarget - CameraLocation;
				FVector VerticalDirection = FVector::CrossProduct(BulletDirection, FMath::VRand());
				VerticalDirection.Normalize();
				//Reset bullet direction based on Scattering angle
				BulletDirection = VerticalDirection * FVector::Distance(CameraLocation, BulletTarget) * FMath::Tan((FMath::FRandRange(-ScatteringAngle, ScatteringAngle) * 3.1415926f) / 180) + BulletDirection;

				EndLocation = CameraLocation + BulletDirection * 10000;

				bool HitSuccess = UKismetSystemLibrary::LineTraceSingle(
					World, CameraLocation, EndLocation, ETraceTypeQuery::TraceTypeQuery1, false,
					IgnoreArray, EDrawDebugTrace::None, HitResult, true);

				if (HitSuccess)//if hit something, turn the bullet to that direction
				{
					APFPlayerCharacter* OtherPlayer = Cast<APFPlayerCharacter>(HitResult.GetActor());
					if (OtherPlayer)
					{
						//if is teammate continue
						if (OtherPlayer->GetTeamID() == MyTeamID)
							continue;

						TargetPlayers.Add(OtherPlayer);

						//Spawn bullet hit player effect
						UNiagaraFunctionLibrary::SpawnSystemAtLocation(World, NSHitPlayer, HitResult.ImpactPoint, HitResult.ImpactNormal.Rotation()
						);
					}
					else
					{
						//Spawn bullet hit object effect
						UNiagaraFunctionLibrary::SpawnSystemAtLocation(World, NSHitHardSurface, HitResult.ImpactPoint, HitResult.ImpactNormal.Rotation());

						//Spawn bullet hit object effect
						UNiagaraFunctionLibrary::SpawnSystemAttached(NSBulletHole, HitResult.GetComponent(), FName("BulletHole"), HitResult.Location,
							HitResult.ImpactNormal.Rotation(),
							EAttachLocation::Type::KeepWorldPosition,
							true);

					}
				}
			}
		}
		else//if use solid bullet, use line trace for bullet correction
		{
			FVector EndLocation = CameraLocation + UKismetMathLibrary::GetForwardVector(SpawnRotation) * 100000;
			BulletTarget = EndLocation;
		}
	}
}

void APFWeaponBaseClient::SetEffectsOnEquiped_Implementation()
{
	//Native implementation
}

void APFWeaponBaseClient::SetEffectsOnDropped_Implementation()
{
	//Native implementation
}

class UAnimSequence* APFWeaponBaseClient::GetFireAnimation() const
{
	return FireAnimation;
}
