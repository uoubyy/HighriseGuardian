// Fill out your copyright notice in the Description page of Project Settings.


#include "PFPlayerController.h"

#include "PFPlayerState.h"
#include "PFAttributeComponent.h"
#include "GameFramework/Controller.h"
#include "PFNakamaSubsystem.h"

void APFPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
}

void APFPlayerController::GameHasEnded( class AActor* EndGameFocus /*= nullptr*/, bool bIsWinner /*= false */ )
{
	Super::GameHasEnded(EndGameFocus , bIsWinner);

	PostExitMatch(bIsWinner);
}

void APFPlayerController::BeginPlay()
{
	Super::BeginPlay();
}

void APFPlayerController::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);

	OldCharacter = aPawn;
}

void APFPlayerController::OnUnPossess()
{
	Super::OnUnPossess();
}

// Client
void APFPlayerController::PostExitMatch_Implementation(bool bIsWinner)
{
	ULocalPlayer* LocalPlayer = GetLocalPlayer();
	if (LocalPlayer)
	{
		UPFNakamaSubsystem* PFNakamaSubsystem = LocalPlayer->GetSubsystem<UPFNakamaSubsystem>();
		if (PFNakamaSubsystem)
		{
			PFNakamaSubsystem->RequestLeaveMatchmaker();
		}
	}
}

void APFPlayerController::SetControlRotationFromServer_Implementation(const FRotator& NewRotation)
{
	SetControlRotation(NewRotation);
}

void APFPlayerController::SetPlayerDisplayName_Implementation(const FString& NewName)
{
	GetPlayerState<APFPlayerState>()->SetPlayerName(NewName);
}

void APFPlayerController::OnRePossessOldCharacter()
{
	if (OldCharacter.IsValid())
	{
		OnPossess(OldCharacter.Get());
	}
}

FString APFPlayerController::GetPlayerSessionID()
{
	if (PlayerSessionID.IsEmpty() && HasAuthority())
	{
		PlayerSessionID = FGuid::NewGuid().ToString();
	}

	return PlayerSessionID;
}

