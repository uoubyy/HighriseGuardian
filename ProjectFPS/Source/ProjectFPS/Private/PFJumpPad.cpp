// Fill out your copyright notice in the Description page of Project Settings.


#include "PFJumpPad.h"
#include <Components/StaticMeshComponent.h>
#include <Components/BoxComponent.h>
#include <Particles/ParticleSystemComponent.h>
#include <Components/PointLightComponent.h>
#include <Components/ArrowComponent.h>
#include <Components/SceneComponent.h>
#include "PFPlayerCharacter.h"

// Sets default values
APFJumpPad::APFJumpPad() :JumpTarget(FVector::ZeroVector)
{
	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);
	SceneRoot->SetMobility(EComponentMobility::Static);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(SceneRoot);

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	TriggerBox->SetupAttachment(SceneRoot);
	TriggerBox->SetCollisionProfileName(FName("Trigger"));

	PadParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("PadParticle"));
	PadParticle->SetupAttachment(SceneRoot);

	PointLight1 = CreateDefaultSubobject<UPointLightComponent>(TEXT("PointLight1"));
	PointLight1->SetupAttachment(SceneRoot);

	SM_Indicator = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SM_Indicator"));
	SM_Indicator->SetupAttachment(SceneRoot);

	Arrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	Arrow->SetupAttachment(SceneRoot);

	JumpTime = 1.0;

	bReplicates = true;
}

void APFJumpPad::BeginPlay()
{
	Super::BeginPlay();

	TargetPosition = GetActorLocation() + JumpTarget;
}

void APFJumpPad::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &APFJumpPad::OnJumpPadBeginOverlap);
}

void APFJumpPad::OnJumpPadBeginOverlap(class UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (HasAuthority())
	{
		APFPlayerCharacter* PlayerCharacter = Cast<APFPlayerCharacter>(OtherActor);

		if (PlayerCharacter)
		{
			FVector	Offset = TargetPosition - PlayerCharacter->GetActorLocation();
			FVector PlayerVelocity = PlayerCharacter->GetVelocity();
			float a = PlayerCharacter->GetGravityZ();

			float v_z = (Offset.Z - 0.5 * a * JumpTime * JumpTime) / JumpTime - PlayerVelocity.Z;
			float v_x = Offset.X / JumpTime - PlayerVelocity.X;
			float v_y = Offset.Y / JumpTime - PlayerVelocity.Y;

			PlayerCharacter->LaunchCharacter(FVector(v_x, v_y, v_z), false, false);
		}
	}
}
