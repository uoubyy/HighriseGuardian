// Copyright FPS Team. All Rights Reserved.


#include "PFPerkBaseComponent.h"

#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UPFPerkBaseComponent::UPFPerkBaseComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;

	SetIsReplicatedByDefault(true);
}


// Called when the game starts
void UPFPerkBaseComponent::BeginPlay()
{
	Super::BeginPlay();

	if (bIsPermanent)
	{
		S_TriggerActivate();
	}

	// ...
	
}

void UPFPerkBaseComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (bIsPermanent)
	{
		S_TriggerDeactivate();
	}

	// ...

}

UFUNCTION()
void UPFPerkBaseComponent::S_TriggerActivate()
{
	if (GetOwner()->GetNetMode() == NM_DedicatedServer)
	{
		S_ActivateForRelatedSystem();
		bIsPerkActive = true;
		S_ActiveEffectOnServer();
		SC_ActiveEffectOnPlayerClient();
		SC_ActiveEffectOnOthersClient();
	}
}

void UPFPerkBaseComponent::SC_ActiveEffectOnPlayerClient_Implementation()
{
	C_ActiveEffectOnPlayerClient();
}

void UPFPerkBaseComponent::SC_ActiveEffectOnOthersClient_Implementation()
{
	C_ActiveEffectOnOthersClient();
}

void UPFPerkBaseComponent::S_TriggerDeactivate()
{
	if (GetOwner()->GetNetMode() == NM_DedicatedServer)
	{
		S_DeactivateForRelatedSystem();
		bIsPerkActive = false;
		S_EndEffectOnServer();
		SC_EndEffectOnPlayerClient();
		SC_EndEffectOnOthersClient();
	}
}

void UPFPerkBaseComponent::SC_EndEffectOnPlayerClient_Implementation()
{
	C_EndEffectOnPlayerClient();
}

void UPFPerkBaseComponent::SC_EndEffectOnOthersClient_Implementation()
{
	C_EndEffectOnOthersClient();
}


void UPFPerkBaseComponent::InitializeComponent()
{
	Super::InitializeComponent();

	InitializePerk();
	UE_LOG(LogTemp, Warning, TEXT("Initialize Perk Component"));
}

void UPFPerkBaseComponent::UninitializeComponent()
{
	Super::UninitializeComponent();

	UninitializePerk();
}

void UPFPerkBaseComponent::S_ActivateForRelatedSystem_Implementation()
{
	//Native implementation
}

void UPFPerkBaseComponent::S_ActiveEffectOnServer_Implementation()
{
	//Native implementation
}

void UPFPerkBaseComponent::C_ActiveEffectOnPlayerClient_Implementation()
{
	//Native implementation
}

void UPFPerkBaseComponent::C_ActiveEffectOnOthersClient_Implementation()
{
	//Native implementation
}

void UPFPerkBaseComponent::S_DeactivateForRelatedSystem_Implementation()
{
	//Native implementation
}

void UPFPerkBaseComponent::S_EndEffectOnServer_Implementation()
{
	//Native implementation
}

void UPFPerkBaseComponent::C_EndEffectOnPlayerClient_Implementation()
{
	//Native implementation
}

void UPFPerkBaseComponent::C_EndEffectOnOthersClient_Implementation()
{
	//Native implementation
}

void UPFPerkBaseComponent::InitializePerk_Implementation()
{
	//Native implementation
}

void UPFPerkBaseComponent::UninitializePerk_Implementation()
{
	//Native implementation
}


void UPFPerkBaseComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UPFPerkBaseComponent, bIsPerkActive);
}