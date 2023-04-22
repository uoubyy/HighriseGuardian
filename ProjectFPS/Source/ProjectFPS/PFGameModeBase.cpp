// Copyright Epic Games, Inc. All Rights Reserved.


#include "PFGameModeBase.h"
#include "PFPlayerController.h"
#include "PFPlayerCharacter.h"
#include "PFPlayerState.h"

#include "PFGameStateBase.h"

#include "PFGameplayFunctionLibrary.h"

#include "Misc/CommandLine.h"
#include <GameFramework/GameMode.h>
#include <Kismet/KismetSystemLibrary.h>
#include <Kismet/GameplayStatics.h>
#include "JsonObjectConverter.h"
#include "GameFramework/GameSession.h" 

void APFGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	GameReadyLeftTime = MaxWaitingTime;
	MatchRoundLeftTime = MatchRoundTime;

	FString ServerLaunchCommand = FCommandLine::Get();

	TArray<FString> Tokens, Switches;
	TMap < FString, FString > OutParams;
	UKismetSystemLibrary::ParseCommandLine(FCommandLine::Get(), Tokens, Switches, OutParams);

	FString HttpRequestPayload;
	for (auto& Param : OutParams)
	{
		UE_LOG(LogTemp, Warning, TEXT("InitGame Game with param %s : %s."), *Param.Key, *Param.Value);
		if(Param.Key == "match_id")
		{ 
			NakamaMatchID = Param.Value;
			HttpRequestPayload = FString::Printf(TEXT("{\"match_id\": \"%s\", \"node\":\"nakama\"}"), *Param.Value);
		}
	}

	if(!HttpRequestPayload.IsEmpty())
	{ 
		FHttpRequestCompleteDelegate HttpRequestCompleteDelegate;
		HttpRequestCompleteDelegate.BindUObject(this, &APFGameModeBase::OnGetMatchStateCompleted);
		// TODO
		// Http Request URL config in ini
		UPFGameplayFunctionLibrary::SendHttpRequestPost("http://127.0.0.1:7350/v2/rpc/server_rpc.get_match_state?http_key=defaulthttpkey&unwrap", HttpRequestPayload, HttpRequestCompleteDelegate);
	}

	UE_LOG(LogTemp, Warning, TEXT("InitGame Game Ready Left Time %f %f."), GameReadyLeftTime, MaxWaitingTime);
}

void APFGameModeBase::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	ErrorMessage.Empty();
	if (HasMatchEnded()) // reject player
	{
		ErrorMessage = "Match has ended, can not join in!";
		FGameModeEvents::GameModePreLoginEvent.Broadcast(this, UniqueId, ErrorMessage);
		return;
	}

	ErrorMessage = GameSession->ApproveLogin(Options);
	FGameModeEvents::GameModePreLoginEvent.Broadcast(this, UniqueId, ErrorMessage);
}

APlayerController* APFGameModeBase::Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal, const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	APlayerController* PlayerController = Super::Login(NewPlayer, InRemoteRole, Portal, Options, UniqueId, ErrorMessage);

	UE_LOG(LogTemp, Warning, TEXT("Player login with options %s"), *Options);

	FString PlayerSessionID = UGameplayStatics::ParseOption(Options, TEXT("session_id"));

	UE_LOG(LogTemp, Warning, TEXT("Player login with PlayerSessionID %s"), *PlayerSessionID);

	APFPlayerController* PFPlayerController = Cast<APFPlayerController>(PlayerController);
	PFPlayerController->SetPlayerSessionID(PlayerSessionID);

	FString CustomName = UGameplayStatics::ParseOption(Options, TEXT("custom_name"));
	ChangeName(PFPlayerController, *CustomName, true);

	return PlayerController;
}

void APFGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer); // NumPlayers increase
	APFPlayerController* PFPlayerController = Cast<APFPlayerController>(NewPlayer);

	APFPlayerState* PFPlayerState = NewPlayer->GetPlayerState<APFPlayerState>();

	int32 AllPlayerNums = TeamANum + TeamBNum;

	int TeamID = 0;
	FString PlayerSessionID = PFPlayerController->GetPlayerSessionID();
	if(!NakamaMatchID.IsEmpty())
	{ 
		TeamID = NakamaMatchState.teamA.Contains(PlayerSessionID) ? 0 : 1;
		PFPlayerState->SetTeamID(TeamID);
		PFPlayerState->SetIndexInTeam(TeamID == 0 ? TeamANum : TeamBNum);

		FNakamaPlayerInfo PlayerInfo = NakamaMatchState.players_info[PlayerSessionID];
		for (const auto& Weapon : PlayerInfo.weapons)
		{
			EWeaponType WeaponType = Weapon.Value;//GetWeaponTypeByID(Weapon.Value);
			PFPlayerState->AddWeaponToInventory(WeaponType);
			UE_LOG(LogTemp, Warning, TEXT("Add Weapon %d To Inventory"), int(WeaponType));
		}
		PFPlayerState->AddWeaponToInventory(EWeaponType::WT_Pistol);

		for (const auto& Ability : PlayerInfo.skills)
		{
			EAbilityType AbilityType = Ability.Value;
			PFPlayerState->AddAbilityToInventory(AbilityType);
			UE_LOG(LogTemp, Warning, TEXT("Add Ability %d To Inventory"), int(AbilityType));
		}

		for (const auto& Perk : PlayerInfo.perks)
		{
			EPerkType PerkType = Perk.Value;
			PFPlayerState->AddPerkToInventory(PerkType);
			UE_LOG(LogTemp, Warning, TEXT("Add Perk %d To Inventory"), int(PerkType));
		}
	}
	else
	{
		// For debug mode
		TeamID = AllPlayerNums % 2;
		PFPlayerState->SetTeamID(TeamID);
		PFPlayerState->SetIndexInTeam(TeamID == 0 ? TeamANum : TeamBNum);

		// register debug weapons to player
		{
			for (auto WeaponType : DebugWeaponList)
			{
				PFPlayerState->AddWeaponToInventory(EWeaponType(WeaponType));
				UE_LOG(LogTemp, Warning, TEXT("Add Weapon %d To Inventory"), int(WeaponType));
			}
		}

		// register all ability to player
		{
			int AbilityType = int(GetNextAbilityType(EAbilityType::AT_None));
			int LastAbilityType = int(EAbilityType::Count);
			for (; AbilityType < LastAbilityType; ++AbilityType)
			{
				PFPlayerState->AddAbilityToInventory(EAbilityType(AbilityType));
				UE_LOG(LogTemp, Warning, TEXT("Add Ability %d To Inventory"), int(AbilityType));
			}
		}

		// register all perk to player
		{
			int PerkType = int(GetNextPerkType(EPerkType::PT_None));
			int LastPerkType = int(EPerkType::Count);
			for (; PerkType < LastPerkType; ++PerkType)
			{
				PFPlayerState->AddPerkToInventory(EPerkType(PerkType));
				UE_LOG(LogTemp, Warning, TEXT("Add Perk %d To Inventory"), int(PerkType));
			}
		}
	}

	RegisterPlayer(PlayerSessionID, PFPlayerController);

	if (TeamID == 0)
		TeamANum++;
	else
		TeamBNum++;

	StartSpawnPlayer(NewPlayer, TeamID);

	UE_LOG(LogTemp, Warning, TEXT("PostLogin Player Nums %d, TeamA Nums %d, TeamB Nums %d."), AllPlayerNums, TeamANum, TeamBNum);
}

void APFGameModeBase::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	if(NumPlayers == 0 && IsRunningDedicatedServer())
		FGenericPlatformMisc::RequestExit(false);
}

void APFGameModeBase::StartToLeaveMap()
{
	// Clean resources
	TeamAPlayerStartPoints.Empty();
	TeamBPlayerStartPoints.Empty();

	Super::StartToLeaveMap();
}

void APFGameModeBase::RegisterPlayer(const FString& PlayerUniqueID, APFPlayerController* PlayerController)
{
	RegisteredPlayers.Add(PlayerUniqueID, PlayerController);
	UE_LOG(LogTemp, Warning, TEXT("Register Player %s %f"), *PlayerUniqueID, GameReadyLeftTime);
}

void APFGameModeBase::OnActorKilled(AActor* VictimActor, AActor* KillerActor)
{
	UE_LOG(LogTemp, Log, TEXT("OnActorKilled: Victim: %s, Killer: %s"), *GetNameSafe(VictimActor), *GetNameSafe(KillerActor));

	// Handle Player death
	APFPlayerCharacter* VictimPlayer = Cast<APFPlayerCharacter>(VictimActor);
	APFPlayerCharacter* KillerPlayer = Cast<APFPlayerCharacter>(KillerActor);

	APFPlayerState* VictimPlayerState = VictimPlayer ? VictimPlayer->GetPlayerState<APFPlayerState>() : nullptr;
	APFPlayerState* KillerPlayerState = KillerPlayer ? KillerPlayer->GetPlayerState<APFPlayerState>() : nullptr;

	APFGameStateBase* FullGameState = GetGameState<APFGameStateBase>();

	if (VictimPlayer && VictimPlayerState)
	{
	
		VictimPlayerState->IncDeadNum();

		AController* Controller = VictimPlayer->GetController();
		//Controller->UnPossess();
		VictimPlayer->OnDeath();

		FullGameState->AddScore(VictimPlayerState->GetTeamID() == 0 ? 1 : 0, 1);

		FTimerHandle TimerHandle_RespawnDelay;
		FTimerDelegate Delegate;
		Delegate.BindUFunction(this, "RespawnPlayerElapsed", Controller);

		float RespawnDelay = 2.0f;
		GetWorldTimerManager().SetTimer(TimerHandle_RespawnDelay, Delegate, RespawnDelay, false);
	}

	// Give Credits for kill
	// Don't credit kills of self
	if (KillerPlayer && KillerPlayer != VictimPlayer)
	{		
		if (KillerPlayerState)
		{
			KillerPlayerState->AddCredits(CreditsPerKill);
			KillerPlayerState->IncKilledNum();
		}
	}

	if (VictimPlayerState && KillerPlayerState)
	{
		FKillRecord KillRecord;

		KillRecord.CauserName = KillerPlayerState->GetPlayerName().Mid(0, 8);
		KillRecord.VictimName = VictimPlayerState->GetPlayerName().Mid(0, 8);
		KillRecord.WeaponType = KillerPlayer->GetCurrentWeaponServer() ? KillerPlayer->GetCurrentWeaponServer()->GetWeaponType() : EWeaponType::WT_None;
		FullGameState->ReportKillRecord(KillRecord);
	}
}

void APFGameModeBase::Tick(float DeltaSeconds)
{
	if (GetMatchState() == MatchState::WaitingToStart)
	{
		GameReadyLeftTime -= DeltaSeconds;
	}

	if (GetMatchState() == MatchState::InProgress)
	{
		MatchRoundLeftTime -= DeltaSeconds;
	}

	//UE_LOG(LogTemp, Warning, TEXT("APFGameModeBase Tick %d %s %f."), (TeamANum + TeamBNum), *MatchState.ToString(), GameReadyLeftTime);

	Super::Tick(DeltaSeconds);
}

void APFGameModeBase::OnGetMatchStateCompleted(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
{
	if(bConnectedSuccessfully)
	{ 
		FString RawString = Response->GetContentAsString();

		// Nakama return return json string 
		{
			RawString.RemoveFromEnd("\"");
			RawString.RemoveFromStart("\"");

			FString SearchText = TEXT("\\");
			FString ReplacementText = TEXT("");
			RawString.ReplaceInline(*SearchText, *ReplacementText);
		}

		UE_LOG(LogTemp, Warning, TEXT("On Get Match State %s."), *RawString);
		
		if (FJsonObjectConverter::JsonObjectStringToUStruct(RawString, &NakamaMatchState))
		{
			UE_LOG(LogTemp, Warning, TEXT("Parse Nakama Match State Success."));
		}
	}
}

bool APFGameModeBase::ReadyToEndMatch_Implementation()
{
	//UE_LOG(LogTemp, Warning, TEXT("ReadyToEndMatch %d %s %f."), (TeamANum + TeamBNum), *MatchState.ToString(), GameReadyLeftTime);
	//if (GetMatchState() == MatchState::WaitingToStart && GameReadyLeftTime < 0.0f)
	//{
	//	if (TeamANum + TeamBNum < MinimumPlayers)
	//	{
	//		//UE_LOG(LogTemp, Warning, TEXT("ReadyToEndMatch End %d %s %f."), (TeamANum + TeamBNum), *MatchState.ToString(), GameReadyLeftTime);
	//		return true;
	//	}
	//}

	if (GetMatchState() == MatchState::InProgress && MatchRoundLeftTime < 0.0f)
	{
		//UE_LOG(LogTemp, Warning, TEXT("ReadyToEndMatch End %d %s %f."), (TeamANum + TeamBNum), *MatchState.ToString(), GameReadyLeftTime);
		return true;
	}

	return false;
}

bool APFGameModeBase::ReadyToStartMatch_Implementation()
{
	//UE_LOG(LogTemp, Warning, TEXT("ReadyToStartMatch %d %s %f."), (TeamANum + TeamBNum), *MatchState.ToString(), GameReadyLeftTime);

	// If bDelayed Start is set, wait for a manual match start
	if (bDelayedStart)
	{
		return false;
	}

	// By default start when we have >= MaxPlayers players
	if (GetMatchState() == MatchState::WaitingToStart)
	{
		return GameReadyLeftTime <= 0.0f;
	}
	return false;
}

void APFGameModeBase::RespawnPlayerElapsed(AController* Controller)
{
	if (ensure(Controller))
	{
		APFPlayerController* PlayerController = Cast<APFPlayerController>(Controller);
		if (PlayerController)
		{
			//PlayerController->OnRePossessOldCharacter();

			APFPlayerCharacter* PlayerCharacter = Cast<APFPlayerCharacter>(PlayerController->GetCharacter());

			APlayerStart* RespawnPoint = FindBestSpawnPointForPlayer(Controller);
			PlayerCharacter->SetActorLocation(RespawnPoint->GetActorLocation());
			PlayerCharacter->SetActorRotation(FRotationMatrix::MakeFromX(RespawnPoint->GetActorForwardVector()).Rotator());
			PlayerCharacter->OnRespawn();
		}
	}
}

void APFGameModeBase::SetMatchState(FName NewState)
{
	Super::SetMatchState(NewState);

	APFGameStateBase* FullGameState = GetGameState<APFGameStateBase>();
	if (FullGameState)
	{
		int32 NewCountDown = 0;
		if (NewState == MatchState::WaitingToStart)
			NewCountDown = MaxWaitingTime;

		if (NewState == MatchState::InProgress)
			NewCountDown = MatchRoundTime;

		FullGameState->SetMatchStageCountDown(NewCountDown);
	}
}

APlayerStart* APFGameModeBase::FindBestSpawnPointForPlayer(AController* Controller)
{
	int32 TeamID = Controller->GetPlayerState<APFPlayerState>()->GetTeamID();
	TArray<APlayerStart*> StartPoints = TeamID == 0 ? TeamAPlayerStartPoints : TeamBPlayerStartPoints;

	float BestDist = 0.0f;
	ensureMsgf(StartPoints.Num() > 0, TEXT("Can not find start points! Check start points Tag, TeamA or TeamB"));
	APlayerStart* BestPoint = StartPoints[NumPlayers % StartPoints.Num()];
	for (auto Point : StartPoints)
	{
		float Dist = 0;
		for (const auto& Player : RegisteredPlayers)
		{
			if (!Player.Value || Player.Value == Controller) continue;

			// Find the player start points away from all the other players
			APFPlayerState* PlayerState = Player.Value->GetPlayerState<APFPlayerState>();
			if (!PlayerState || PlayerState->GetTeamID() != TeamID) continue;

			if (!Player.Value->GetPawn()) continue;

			FVector Offset = Player.Value->GetPawn()->GetActorLocation() - Point->GetActorLocation();
			Dist += Offset.Size();
		}

		if (Dist > BestDist)
		{
			BestPoint = Point;
			BestDist = Dist;
		}
	}

	return BestPoint;
}
