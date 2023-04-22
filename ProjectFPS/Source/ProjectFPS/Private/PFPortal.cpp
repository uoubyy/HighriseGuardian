// Copyright FPS Team. All Rights Reserved.


#include "PFPortal.h"
#include "PFPlayerCharacter.h"
#include "PFPlayerController.h"
#include <Components/StaticMeshComponent.h>
#include <Components/BoxComponent.h>
#include <Components/SceneComponent.h>
#include <GameFramework/CharacterMovementComponent.h>

// Sets default values
APFPortal::APFPortal()
{
	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);
	SceneRoot->SetMobility(EComponentMobility::Static);

	Niagara = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Niagara"));
	Niagara->SetupAttachment(SceneRoot);

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	TriggerBox->SetupAttachment(SceneRoot);
	TriggerBox->SetCollisionProfileName(FName("Trigger"));

	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

void APFPortal::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &APFPortal::OnPortalBeginOverlap);
}

void APFPortal::OnPortalBeginOverlap(class UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (HasAuthority())
	{
		APFPlayerCharacter* PlayerCharacter = Cast<APFPlayerCharacter>(OtherActor);

		if (PlayerCharacter)
		{
			FRotator characterDeltaRotator = TeleportTarget.Rotator() + this->GetActorRotation() - PlayerCharacter->GetTransform().Rotator();
			characterDeltaRotator.Normalize();

			PlayerCharacter->TeleportTo(TeleportTarget.GetLocation() + this->GetActorLocation(), TeleportTarget.Rotator() + this->GetActorRotation());
			APFPlayerController* PlayerController = Cast<APFPlayerController>(PlayerCharacter->GetController());
			if (PlayerController) PlayerController->SetControlRotationFromServer(TeleportTarget.Rotator() + this->GetActorRotation());
			float newVelocityZ = PlayerCharacter->GetCharacterMovement()->Velocity.Z >= 0 ? PlayerCharacter->GetCharacterMovement()->Velocity.Z : -PlayerCharacter->GetCharacterMovement()->Velocity.Z;
			PlayerCharacter->GetCharacterMovement()->Velocity = characterDeltaRotator.RotateVector(PlayerCharacter->GetCharacterMovement()->Velocity);
			// flip velocity.z if its direction is down 
			// PlayerCharacter->GetCharacterMovement()->Velocity.Z = newVelocityZ;
		}
	}
}