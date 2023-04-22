// Fill out your copyright notice in the Description page of Project Settings.


#include "PFGameplayFunctionLibrary.h"
#include <GameFramework/PlayerState.h>
#include "PFPlayerState.h"
#include "PFAttributeComponent.h"

#include "../PFGameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "PFPickUpBase.h"

#include "Engine/World.h"

bool UPFGameplayFunctionLibrary::ApplyDamage(AActor* DamageCauser, AActor* TargetActor, float DamageAmount)
{
	UPFAttributeComponent* AttributeComp = UPFAttributeComponent::GetAttributes(TargetActor);
	if (AttributeComp)
	{
		return AttributeComp->ApplyHealthChange(DamageCauser, -DamageAmount);
	}
	return false;
}

bool UPFGameplayFunctionLibrary::AddHealth(AActor* BenefitsCauser, AActor* TargetActor, float HealthAmount)
{
	UPFAttributeComponent* AttributeComp = UPFAttributeComponent::GetAttributes(TargetActor);
	if (AttributeComp)
	{
		return AttributeComp->ApplyHealthChange(BenefitsCauser, HealthAmount);
	}
	return false;
}

TArray<APawn*> UPFGameplayFunctionLibrary::GetAllTeammates(APawn* Player)
{
	TArray<APawn*> Teammates;

	int32 TargetTeam = -1;
	{
		APlayerState* PS = Player->GetPlayerState();
		if (PS)
		{
			APFPlayerState* PlayerState = Cast<APFPlayerState>(PS);
			if (PlayerState)
				TargetTeam = PlayerState->GetTeamID();
		}
	}

	// Only Game State exists in client
	// TODO optimization
	// Store team info in GameState directly
	if (TargetTeam >= 0 && Player->GetWorld())
	{
		APFGameStateBase* GS = Cast<APFGameStateBase>(Player->GetWorld()->GetGameState());
		if (GS)
		{
			for (APlayerState* PS : GS->PlayerArray)
			{
				APFPlayerState* PlayerState = Cast<APFPlayerState>(PS);
				APawn* Pawn = PlayerState->GetPawn();
				if (Pawn == Player) continue;

				if (PlayerState->GetTeamID() == TargetTeam)
					Teammates.Add(Pawn);
			}
		}
	}

	return Teammates;
}

TArray<APawn*> UPFGameplayFunctionLibrary::GetAllEnemies(APawn* Player)
{
	TArray<APawn*> Enemies;

	int32 TargetTeam = -1;
	{
		APlayerState* PS = Player->GetPlayerState();
		if (PS)
		{
			APFPlayerState* PlayerState = Cast<APFPlayerState>(PS);
			if (PlayerState)
				TargetTeam = PlayerState->GetTeamID();
		}
	}

	// Only Game State exists in client
	// TODO optimization
	// Store team info in GameState directly
	if (TargetTeam >= 0 && Player->GetWorld())
	{
		APFGameStateBase* GS = Cast<APFGameStateBase>(Player->GetWorld()->GetGameState());
		if (GS)
		{
			for (APlayerState* PS : GS->PlayerArray)
			{
				APFPlayerState* PlayerState = Cast<APFPlayerState>(PS);
				if (PlayerState->GetTeamID() != TargetTeam)
					Enemies.Add(PlayerState->GetPawn());
			}
		}
	}

	return Enemies;
}

void UPFGameplayFunctionLibrary::SendHttpRequestPost(const FString URL, const FString Payload, FHttpRequestCompleteDelegate HttpRequestCompleteDelegate)
{
	TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(URL);
	HttpRequest->SetVerb("POST");
	HttpRequest->SetHeader("Content-Type", TEXT("application/json"));
	HttpRequest->SetHeader(TEXT("Accepts"), TEXT("application/json"));
	HttpRequest->SetContentAsString(Payload);
	FHttpRequestCompleteDelegate& RequestCompleteDelegate = HttpRequest->OnProcessRequestComplete();
	RequestCompleteDelegate = HttpRequestCompleteDelegate;
	HttpRequest->ProcessRequest();

	UE_LOG(LogTemp, Warning, TEXT("SendHttpRequestPost with payload %s."), *Payload);
}

void UPFGameplayFunctionLibrary::SendHttpRequestGet(const FString URL, FHttpRequestCompleteDelegate HttpRequestCompleteDelegate)
{
	TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(URL);
	HttpRequest->SetVerb("GET");
	HttpRequest->SetHeader("Content-Type", TEXT("application/json"));
	HttpRequest->SetHeader(TEXT("Accepts"), TEXT("application/json"));

	FHttpRequestCompleteDelegate& RequestCompleteDelegate = HttpRequest->OnProcessRequestComplete();
	RequestCompleteDelegate = HttpRequestCompleteDelegate;

	//RequestCompleteDelegate.BindLambda([](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
	//	{
	//		UE_LOG(LogTemp, Warning, TEXT("SendHttpRequestGet Response %s"), *Response->GetContentAsString());
	//	});

	HttpRequest->ProcessRequest();
}

TArray<TSoftObjectPtr<AActor>> UPFGameplayFunctionLibrary::GetAllPickupsOfType(const UWorld* WorldContext, EPowerupType PickupType)
{
	TArray<TSoftObjectPtr<AActor>> PickupsOfType;
	TArray<AActor*> Pickups;

	UGameplayStatics::GetAllActorsOfClass(WorldContext, APFPickUpBase::StaticClass(), Pickups);

	for (auto Actor : Pickups)
	{
		APFPickUpBase* Pickup = Cast<APFPickUpBase>(Actor);
		if (Pickup && Pickup->GetPowerupType() == PickupType)
		{
			PickupsOfType.Add(Actor);
		}
	}

	return PickupsOfType;
}

TArray<TSoftObjectPtr<AActor>> UPFGameplayFunctionLibrary::GetAllPickups(const UWorld* WorldContext)
{
	TArray<TSoftObjectPtr<AActor>> PickupsOfType;
	TArray<AActor*> Pickups;

	UGameplayStatics::GetAllActorsOfClass(WorldContext, APFPickUpBase::StaticClass(), Pickups);

	for (auto Actor : Pickups)
	{
		PickupsOfType.Add(Actor);
	}

	return PickupsOfType;
}

FString UPFGameplayFunctionLibrary::ConvertNetModeEnumToString(ENetMode NetMode)
{
	switch (NetMode)
	{
	case NM_Standalone:
		return "ENetMode::NM_Standalone";
		break;
	case NM_DedicatedServer:
		return "ENetMode::NM_DedicatedServer";
		break;
	case NM_ListenServer:
		return "ENetMode::NM_ListenServer";
		break;
	case NM_Client:
		return "ENetMode::NM_Client";
		break;
	case NM_MAX:
		return "ENetMode::NM_MAX";
		break;
	default:
		break;
	}
	return "ENetMode::Unknow";
}

FString UPFGameplayFunctionLibrary::ConvertNetRoleEnumToString(ENetRole NetRole)
{
	switch (NetRole)
	{
	case ROLE_None:
		return "ENetRole::ROLE_None";
		break;
	case ROLE_SimulatedProxy:
		return "ENetRole::ROLE_SimulatedProxy";
		break;
	case ROLE_AutonomousProxy:
		return "ENetRole::ROLE_AutonomousProxy";
		break;
	case ROLE_Authority:
		return "ENetRole::ROLE_Authority";
		break;
	case ROLE_MAX:
		return "ENetRole::ROLE_MAX";
		break;
	default:
		break;
	}
	return "ENetRole::Unknow";
}