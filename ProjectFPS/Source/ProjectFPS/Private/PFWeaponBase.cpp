// Fill out your copyright notice in the Description page of Project Settings.


#include "PFWeaponBase.h"
#include "../Public/NiagaraComponent.h"

#include "GameFramework/Actor.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"

APFWeaponBase* APFWeaponBase::SpawnWeapon(UWorld* World, UClass* WeaponClass, FTransform const& Transform, AActor* WeaponOwner)
{
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.Owner = WeaponOwner;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	APFWeaponBase* NewWeapon = World->SpawnActor<APFWeaponBase>(WeaponClass, Transform, SpawnInfo);
	NewWeapon->Ammo = NewWeapon->MaxAmmo;

	return NewWeapon;
}


// Sets default values
APFWeaponBase::APFWeaponBase()
{
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(FName("WeaponMesh"));
	RootComponent = WeaponMesh;
	WeaponMesh->SetOnlyOwnerSee(true);

	MuzzleFlash = CreateDefaultSubobject<UNiagaraComponent>(FName("MuzzleFlash"));
	MuzzleFlash->SetupAttachment(RootComponent);
}

void APFWeaponBase::BeginPlay()
{
	Super::BeginPlay();

	//Init LastFireTime
	LastFireTime = UGameplayStatics::GetRealTimeSeconds(GetWorld());
}

// SocketName is deprecated, every weapon can set target socket
void APFWeaponBase::OnWeaponEquiped(UPrimitiveComponent* TargetPlayer, FName SocketName)
{
	SetActorHiddenInGame(false);

	this->K2_AttachToComponent(TargetPlayer, AttachedSocketName, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, true);

	WeaponStatus = EWeaponStatusType::WST_Equipped;
}

void APFWeaponBase::OnWeaponDropped()
{
	FTimerHandle TimerHandle;
	FTimerDelegate TimerDelegate;

	TimerDelegate.BindLambda([this]()
	{
		SetActorHiddenInGame(true);
		this->WeaponStatus = EWeaponStatusType::WST_NotEquipped;
	});

	GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, 0.3f, false);
}

void APFWeaponBase::OnReLoad()
{
	Ammo = MaxAmmo;
}

bool APFWeaponBase::OnAddAmmo(int AmmoAmount)
{
	if (Ammo == MaxAmmo) return false;

	Ammo = FMath::Min<int>(Ammo + AmmoAmount, MaxAmmo);
	return true;
}

bool APFWeaponBase::CanFire()
{
	return Ammo > 0;
}

void APFWeaponBase::OnPreDrop()
{
	WeaponStatus = EWeaponStatusType::WST_PreDrop;
}

void APFWeaponBase::OnPreEquip()
{
	WeaponStatus = EWeaponStatusType::WST_PreEquip;
}

void APFWeaponBase::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(APFWeaponBase, MaxAmmo, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(APFWeaponBase, Ammo, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(APFWeaponBase, IsAuto, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(APFWeaponBase, IsLineTraceWeapon, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(APFWeaponBase, WeaponDamage, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(APFWeaponBase, FireRate, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(APFWeaponBase, BulletsPerFire, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(APFWeaponBase, ScatteringAngle, COND_OwnerOnly);
}

