// Fill out your copyright notice in the Description page of Project Settings.


#include "PFAbilityBaseComponent.h"

#include "Net/UnrealNetwork.h"
#include "Engine/World.h"
#include "PFPlayerCharacter.h"
#include "PFGameplayFunctionLibrary.h"
#include "UI/PFInGameHUD.h"

// Sets default values for this component's properties
UPFAbilityBaseComponent::UPFAbilityBaseComponent()
{
	bIsCharacterSelfAOT = true;
	bIsBeingAttached = false;
	bIsInAnticipation = false;
	bIsInUse = false;
	bIsReady = true;
	Duration = 0.0f;
	Anticipation = 0.0f;
	CanCeaseAnticipation = false;
	CoolDownTime = 0.0f;
	bWantsInitializeComponent = true;

	SetIsReplicatedByDefault(true);
}

void UPFAbilityBaseComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!bIsCharacterSelfAOT) Duration = 0;

	if (GetNetMode() == ENetMode::NM_Client)
	{
		APFPlayerCharacter* OwnerCharacter = Cast<APFPlayerCharacter>(GetOwner());

		UPFInGameHUD* InGameHUD = OwnerCharacter->InGameHUD;
		if(InGameHUD)
		{ 
			OnAbilityTrigger.AddDynamic(InGameHUD, &UPFInGameHUD::OnAbilityTrigger);
			OnAbilityStart.AddDynamic(InGameHUD, &UPFInGameHUD::OnAbilityStart);
			OnAbilityEnd.AddDynamic(InGameHUD, &UPFInGameHUD::OnAbilityEnd);
			OnAbilityCeased.AddDynamic(InGameHUD, &UPFInGameHUD::OnAbilityEnd);
		}
	}
}

void UPFAbilityBaseComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (GetNetMode() == ENetMode::NM_Client)
	{
		APFPlayerCharacter* OwnerCharacter = Cast<APFPlayerCharacter>(GetOwner());

		UPFInGameHUD* InGameHUD = OwnerCharacter->InGameHUD;
		if (InGameHUD)
		{
			OnAbilityTrigger.RemoveDynamic(InGameHUD, &UPFInGameHUD::OnAbilityTrigger);
			OnAbilityStart.RemoveDynamic(InGameHUD, &UPFInGameHUD::OnAbilityStart);
			OnAbilityEnd.RemoveDynamic(InGameHUD, &UPFInGameHUD::OnAbilityEnd);
			OnAbilityCeased.RemoveDynamic(InGameHUD, &UPFInGameHUD::OnAbilityEnd);
		}
	}
}

void UPFAbilityBaseComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UPFAbilityBaseComponent, bIsInAnticipation, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UPFAbilityBaseComponent, bIsInUse, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UPFAbilityBaseComponent, bIsReady, COND_OwnerOnly);

	DOREPLIFETIME_CONDITION(UPFAbilityBaseComponent, Duration, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UPFAbilityBaseComponent, Anticipation, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UPFAbilityBaseComponent, CoolDownTime, COND_OwnerOnly);
}

void UPFAbilityBaseComponent::CS_TriggerUsing_Implementation()
{
	if (bIsReady)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("Ability %s triggered. Anticipation %f, Duration %f, CoodDownTime %f."), *GetName(), Anticipation, Duration, CoolDownTime));
		bIsReady = false;
		bIsInAnticipation = true;
		SC_OnAnticipation();
		if (Anticipation) GetWorld()->GetTimerManager().SetTimer(AnticipationTimer, this, &UPFAbilityBaseComponent::S_OnStartUsing, Anticipation, false);
		else S_OnStartUsing();
	}
}

void UPFAbilityBaseComponent::SC_OnAnticipation_Implementation()
{
	OnAbilityTrigger.Broadcast(GetOwner(), AbilityType, Anticipation, Duration, CoolDownTime);
	C_OnAnticipation();
}

void UPFAbilityBaseComponent::S_OnStartUsing()
{
	bIsInAnticipation = false;
	bIsInUse = true;
	S_StartUsing();
	SC_OnStartUsing();
	if (Duration) GetWorld()->GetTimerManager().SetTimer(DurationTimer, this, &UPFAbilityBaseComponent::S_OnEndUsing, Duration, false);
	else S_OnEndUsing();
}

void UPFAbilityBaseComponent::SC_OnStartUsing_Implementation()
{
	// if (GetWorld()->GetNetMode() == NM_Client) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("Client Now SC_OnStartUsing")));

	OnAbilityStart.Broadcast(GetOwner(), AbilityType, Duration, CoolDownTime);
	C_OnStartUsing();
}

void UPFAbilityBaseComponent::S_OnEndUsing()
{
	bIsInUse = false;
	if (CoolDownTime) GetWorld()->GetTimerManager().SetTimer(CoolDownTimer, this, &UPFAbilityBaseComponent::S_ResetReady, CoolDownTime, false);
	else S_ResetReady();
	S_EndUsing();
	SC_OnEndUsing();
}

void UPFAbilityBaseComponent::SC_OnEndUsing_Implementation()
{
	OnAbilityEnd.Broadcast(GetOwner(), AbilityType, CoolDownTime);
	C_OnEndUsing();
}

void UPFAbilityBaseComponent::CS_TriggerCeaseUsing_Implementation()
{
	if (bIsInUse)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("Ability %s ceased."), *GetName()));

		S_OnCeaseUsing();
	}
}

void UPFAbilityBaseComponent::S_OnCeaseUsing()
{
	if (Duration) GetWorld()->GetTimerManager().ClearTimer(DurationTimer);
	bIsInUse = false;
	if (CoolDownTime) GetWorld()->GetTimerManager().SetTimer(CoolDownTimer, this, &UPFAbilityBaseComponent::S_ResetReady, CoolDownTime, false);
	else S_ResetReady();
	S_CeaseUsing();
	SC_OnCeaseUsing();
}

void UPFAbilityBaseComponent::SC_OnCeaseUsing_Implementation()
{
	OnAbilityCeased.Broadcast(GetOwner(), AbilityType, CoolDownTime);
	C_OnCeaseUsing();
}

void UPFAbilityBaseComponent::CS_TriggerCeaseAnticipation_Implementation()
{
	if (bIsInAnticipation && CanCeaseAnticipation)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("Ability Anticipation %s ceased."), *GetName()));
		GetWorld()->GetTimerManager().ClearTimer(AnticipationTimer);
		S_OnStartUsing();
	}
}

void UPFAbilityBaseComponent::S_ResetReady()
{
	bIsReady = true;
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("Ability %s ready."), *GetName(), Anticipation));
}

void UPFAbilityBaseComponent::InitializeComponent()
{
	Super::InitializeComponent();
	InitializeAbility();
}
