// Fill out your copyright notice in the Description page of Project Settings.


#include "PFPickUpBase.h"
#include <Components/SceneComponent.h>
#include <Components/StaticMeshComponent.h>
#include <Components/BoxComponent.h>
#include "PFPlayerCharacter.h"
#include "PFGameplayFunctionLibrary.h"

// TODO
// May change to Pickable Interface

// Sets default values
APFPickUpBase::APFPickUpBase()
{
	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);
	SceneRoot->SetMobility(EComponentMobility::Static);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(SceneRoot);

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	TriggerBox->SetupAttachment(SceneRoot);
	TriggerBox->SetCollisionProfileName(FName("Trigger"));

	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
}

void APFPickUpBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &APFPickUpBase::OnPortalBeginOverlap);

	OnItemActived();
}

void APFPickUpBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if(IsPickupAvailable) return;

	RespawnCDLeftTime -= DeltaSeconds;
}

void APFPickUpBase::OnPortalBeginOverlap(class UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	bool Consumed = false;

	if (HasAuthority())
	{
		APFPlayerCharacter* PlayerCharacter = Cast<APFPlayerCharacter>(OtherActor);

		if (PlayerCharacter)
		{
			switch (PowerupType)
			{
			case EPowerupType::PT_AmmoBox:
			{
				APFWeaponBaseServer* WeaponServer = PlayerCharacter->GetWeaponServerOfType(TargetweaponType);
				// APFWeaponBase* WeaponClient = PlayerCharacter->GetWeaponClientOfType(TargetweaponType);

				Consumed = WeaponServer ? WeaponServer->OnAddAmmo(AmmoBenefits) : false;

				if(Consumed)
					PlayerCharacter->ClientAddAmmo(AmmoBenefits);
			}
			break;
			case EPowerupType::PT_HealthBox:
			{
				Consumed = UPFGameplayFunctionLibrary::AddHealth(this, OtherActor, BenefitsAmount);
			}
			break;
			default:
				break;
			}
		}

		if (Consumed)
		{
			OnItemConsumed();

			FTimerDelegate TimerDelegate;

			TimerDelegate.BindLambda([&]()
				{
					// SetActorHiddenInGame(false);
					OnItemActived();
				});

			GetWorld()->GetTimerManager().SetTimer(TimerHandle_Respawn, TimerDelegate, RespawnTime, false);
		}
	}
}

// TODO: Yanyi Bao
EWeaponType APFPickUpBase::GetTargetWeaponType() const
{
	if(PowerupType == EPowerupType::PT_AmmoBox)
		return TargetweaponType;
	return EWeaponType::WT_None;
}

void APFPickUpBase::OnItemConsumed_Implementation()
{
	RespawnCDLeftTime = RespawnTime;
	IsPickupAvailable = false;
	TriggerBox->SetCollisionProfileName(FName("IgnoreAll"));

	if(!HasAuthority())
	{ 
		OnItemConsumedClient();
	}
}

void APFPickUpBase::OnItemActived_Implementation()
{
	IsPickupAvailable = true;
	RespawnCDLeftTime = 0;
	TriggerBox->SetCollisionProfileName(FName("Trigger"));

	if (!HasAuthority())
	{ 
		OnItemActivedClient();
	}
}
