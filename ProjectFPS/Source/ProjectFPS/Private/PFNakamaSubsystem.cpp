// Copyright FPS Team. All Rights Reserved.


#include "PFNakamaSubsystem.h"
#include "PFSaveGameSettings.h"
#include <JsonObjectConverter.h>
#include "PFPopupSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/PlayerController.h"
#include "PFGameplayFunctionLibrary.h"
#include "PFGameConstant.h"

#define STEAM_ENABLE

#ifdef STEAM_ENABLE
#include "ThirdParty/Steamworks/Steamv151/sdk/public/steam/steam_api.h"
#endif

#define OPCODE_JOIN 100
#define OPCODE_LEAVE 101

#define OPCODE_PLAYER_READY 200
#define OPCODE_MATCH_READY 201
#define OPCODE_MATCH_END 202

#define OPCODE_PLAYER_SET_WEAPON 301
#define OPCODE_PLAYER_SET_SKILL 302
#define OPCODE_PLAYER_SET_PERK 303

#define OPCODE_PLAYER_VOTE_MAP 401
#define OPCODE_PLAYER_CHANGE_TEAM 501

void UPFNakamaSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// Device authentication
	AuthenticationSuccessDelegate.AddDynamic(this, &UPFNakamaSubsystem::OnAuthenticationSuccess);
	NakamaClientErrorDelegate.AddDynamic(this, &UPFNakamaSubsystem::OnNakamaClientError);

	// Realtime Connection
	ConnectionSuccessDelegate.AddDynamic(this, &UPFNakamaSubsystem::OnRealtimeClientConnectionSuccess);
	ConnectionErrorDelegate.AddDynamic(this, &UPFNakamaSubsystem::OnRealtimeClientConnectionError);
	NakamaDisconnectedDelegate.AddDynamic(this, &UPFNakamaSubsystem::OnRealtimeClientDisconnected);

	// Account
	GetAccountSuccessDelegate.AddDynamic(this, &UPFNakamaSubsystem::OnGetAccountSuccess);

	// Match Maker Relevant
	AddMatchmakerSuccessDelegate.AddDynamic(this, &UPFNakamaSubsystem::OnAddMatchmakerSuccess);
	MatchmakerMatchedSuccessDelegate.AddDynamic(this, &UPFNakamaSubsystem::OnMatchmakerMatchedSuccess);

	// Create Match
	CreateMatchSuccessDelegate.AddDynamic(this, &UPFNakamaSubsystem::OnCreateMatchSuccess);

	JoinMatchSuccessDelegate.AddDynamic(this, &UPFNakamaSubsystem::OnJoinMatchSuccess);

	ReceivedMatchPresenceDelegate.AddDynamic(this, &UPFNakamaSubsystem::OnReceivedMatchPresence);

	// Match Data
	ReceivedMatchDataDelegate.AddDynamic(this, &UPFNakamaSubsystem::OnReceivedMatchData);
	ListMatchesSuccessDelegate.AddDynamic(this, &UPFNakamaSubsystem::OnListMatchesSuccess);

	// Notification
	NotificationReceivedDelegate.AddDynamic(this, &UPFNakamaSubsystem::OnReceivedNotification);

	// Realtime Client Error
	NakamaRealtimeErrorDelegate.AddDynamic(this, &UPFNakamaSubsystem::OnNakamaRealtimeError);

	IsReleaseVersion = false;
}

void UPFNakamaSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void UPFNakamaSubsystem::NakamaLogin()
{
	if (FParse::Value(FCommandLine::Get(), TEXT("NakamaHost"), NakamaHost))
	{
		NakamaPort = 7350;
		AuthenticateDevice();
		UE_LOG(LogTemp, Warning, TEXT("Get Nakama Server Config %s:%d"), *NakamaHost, NakamaPort);
	}
	else
	{
		GetServerConfig();
	}
}

void UPFNakamaSubsystem::GetServerConfig()
{
	FHttpRequestCompleteDelegate HttpRequestCompleteDelegate;
	HttpRequestCompleteDelegate.BindLambda([&](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful) {
		if (bWasSuccessful)
		{
			TSharedPtr<FJsonObject> JsonParsed;
			TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(Response->GetContentAsString());
			if (FJsonSerializer::Deserialize(JsonReader, JsonParsed))
			{
				UE_LOG(LogTemp, Warning, TEXT("Get Server Config %s"), *JsonParsed->GetStringField("NakamaHost"));
				NakamaHost = JsonParsed->GetStringField("NakamaHost");
				NakamaPort = JsonParsed->GetIntegerField("NakamaPort");
				JsonParsed->TryGetBoolField("ReleaseVersion", IsReleaseVersion);
			}
		}

		AuthenticateDevice();
		});

	UPFGameplayFunctionLibrary::SendHttpRequestGet("https://raw.githubusercontent.com/uoubyy/HighriseGuardian/dev/Configs/config.json", HttpRequestCompleteDelegate);
}

void UPFNakamaSubsystem::AuthenticateDevice()
{
	if (NakamaRealtimeClient && NakamaRealtimeClient->bIsActive)
	{
		UE_LOG(LogTemp, Warning, TEXT("Device already authenticated, no need to authenticate again."));
		return;
	}

	FString ServerKey = TEXT("defaultkey");

	NakamaClient = UNakamaClient::CreateDefaultClient(ServerKey, NakamaHost, NakamaPort);

#ifdef STEAM_ENABLE
	GetSteamAccountInfo();
#endif

	if (NakamaDeviceId.IsEmpty())
	{
		APlayerController* LocalPlayerController = GetWorld()->GetFirstPlayerController();
		if (LocalPlayerController)
		{
			NakamaDeviceId = LocalPlayerController->GetPlayerState<APlayerState>()->GetPlayerName();
			PlayerDisplayName = NakamaDeviceId;
		}
	}

	bool bCreate = true;
	TMap<FString, FString> Vars;

	// Authenticate with the Nakama server using Device Authentication.
	NakamaClient->AuthenticateDevice(NakamaDeviceId, PlayerDisplayName, bCreate, Vars, AuthenticationSuccessDelegate, NakamaClientErrorDelegate);
}

void UPFNakamaSubsystem::OnAuthenticationSuccess(UNakamaSession* LoginData)
{
	UE_LOG(LogTemp, Log, TEXT("OnAuthenticationSuccess succeeded."));

	NakamaSession = LoginData;
	NakamaClient->GetUserAccount(NakamaSession, GetAccountSuccessDelegate, NakamaClientErrorDelegate);
	NakamaCreateRealTimeConnection();
}

void UPFNakamaSubsystem::NakamaCreateRealTimeConnection()
{
	NakamaRealtimeClient = NakamaClient->SetupRealtimeClient(NakamaSession);

	NakamaRealtimeClient->DisconnectedEvent = NakamaDisconnectedDelegate;
	NakamaRealtimeClient->SetListenerDisconnectCallback();

	NakamaRealtimeClient->MatchmakerMatchMatched = MatchmakerMatchedSuccessDelegate;
	NakamaRealtimeClient->SetListenerMatchmakerMatchedCallback();

	NakamaRealtimeClient->MatchmakerPresenceCallback = ReceivedMatchPresenceDelegate;
	NakamaRealtimeClient->SetListenerMatchPresenceCallback();

	NakamaRealtimeClient->NotificationReceived = NotificationReceivedDelegate;
	NakamaRealtimeClient->SetListenerNotificationsCallback();

	NakamaRealtimeClient->MatchDataCallback = ReceivedMatchDataDelegate;
	NakamaRealtimeClient->SetListenerMatchDataCallback();

	NakamaRealtimeClient->Connect(ConnectionSuccessDelegate, ConnectionErrorDelegate);
}

void UPFNakamaSubsystem::DestroyClient()
{
	if (NakamaRealtimeClient)
	{
		NakamaRealtimeClient->Destroy();
		NakamaRealtimeClient = nullptr;
	}

	if (NakamaClient)
	{
		NakamaClient->Destroy();
		NakamaClient = nullptr;
	}
}

void UPFNakamaSubsystem::OnNakamaClientError(const FNakamaError& Error)
{
	UE_LOG(LogTemp, Error, TEXT("NakamaClientError: %s"), *Error.Message);
	FInGameError ErrorData;
	ErrorData.ErrorCode = int32(Error.Code);
	ErrorData.Message = Error.Message;
	ShowNakamaError(ErrorData);
}

void UPFNakamaSubsystem::OnRealtimeClientConnectionSuccess()
{
	UE_LOG(LogTemp, Log, TEXT("Socket connection succeeded."));
}

void UPFNakamaSubsystem::OnRealtimeClientConnectionError()
{
	UE_LOG(LogTemp, Log, TEXT("Socket connection failed."));
}

void UPFNakamaSubsystem::OnRealtimeClientDisconnected(const FNakamaDisconnectInfo& DisconnectInfo)
{
	FInGameError ErrorData;
	ErrorData.ErrorCode = DisconnectInfo.Code;
	ErrorData.Message = DisconnectInfo.Reason;
	ShowNakamaError(ErrorData);
}

void UPFNakamaSubsystem::OnGetAccountSuccess(const FNakamaAccount& AccountData)
{

}

void UPFNakamaSubsystem::OnAddMatchmakerSuccess(FString Ticket)
{
	UE_LOG(LogTemp, Log, TEXT("Successfully joined matchmaker: %s"), *Ticket);
	NakamaTicket = Ticket;
}

void UPFNakamaSubsystem::OnCreateMatchSuccess(FNakamaMatch Match)
{
	NakamaMatchID = Match.MatchId;
}

void UPFNakamaSubsystem::OnHttpCreateMatchResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
{
	if (bConnectedSuccessfully)
	{
		FString JsonRaw = Response->GetContentAsString();

		TSharedPtr<FJsonObject> JsonParsed;
		TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(JsonRaw);
		if (FJsonSerializer::Deserialize(JsonReader, JsonParsed))
		{
			FString MatchId = JsonParsed->GetStringField("match_id");

			UE_LOG(LogTemp, Warning, TEXT("OnHttpCreateMatchResponse Match ID %s."), *MatchId);

			RequestJoinMatch(MatchId);
		}

		UE_LOG(LogTemp, Warning, TEXT("OnHttpCreateMatchResponse %s."), *JsonRaw);
	}
}

void UPFNakamaSubsystem::OnMatchmakerMatchedSuccess(const FNakamaMatchmakerMatched& Match)
{
	RequestJoinMatch(Match.MatchId);
}

void UPFNakamaSubsystem::OnJoinMatchSuccess(FNakamaMatch Match)
{
	CurrentMatch = Match;
	NakamaMatchID = Match.MatchId;
	UE_LOG(LogTemp, Log, TEXT("Successfully joined match: %s"), *Match.MatchId);
}

void UPFNakamaSubsystem::OnJoinMatchError(const FNakamaRtError& Error)
{
	UE_LOG(LogTemp, Error, TEXT("Error joining match: %s"), *Error.Message);
	FInGameError ErrorData;
	ErrorData.ErrorCode = int32(Error.Code);
	ErrorData.Message = Error.Message;
	ShowNakamaError(ErrorData);
}

void UPFNakamaSubsystem::OnNakamaRealtimeError(const FNakamaRtError& Error)
{
	UE_LOG(LogTemp, Error, TEXT("Nakama Realtime Error: %s"), *Error.Message);
	FInGameError ErrorData;
	ErrorData.ErrorCode = int32(Error.Code);
	ErrorData.Message = Error.Message;
	ErrorData.OnPopupConfirmDelegate.AddDynamic(this, &UPFNakamaSubsystem::NakamaLogin);
	// ErrorData.OnPopupCancelDelegate.a
	ShowNakamaError(ErrorData);
}

void UPFNakamaSubsystem::OnListMatchesSuccess(const FNakamaMatchList& MatchList)
{
	for (int16 i = 0; i < MatchList.Matches.Num(); i++)
	{
		FNakamaMatch Match = MatchList.Matches[i];
		UE_LOG(LogTemp, Log, TEXT("Match Info %s:%d players"), *Match.MatchId, Match.Size);
	}
}

void UPFNakamaSubsystem::OnReceivedMatchData(const FNakamaMatchData& MatchData)
{
	switch (MatchData.OpCode)
	{
		case OPCODE_JOIN:
		{
			TArray<FNakamaMsgJoin> JoinPlayers;
			if (FJsonObjectConverter::JsonArrayStringToUStruct(MatchData.Data, &JoinPlayers))
			{
				UE_LOG(LogTemp, Log, TEXT("On Received Match Data: Player Join Match %s"), *MatchData.Data);
				PlayersJoinMatchDelegate.Broadcast(JoinPlayers);

				for (const auto& Player : JoinPlayers)
				{
					// Our UE4 dedicated server use session id to identity player
					if (Player.user_id == NakamaSession->SessionData.UserId)
					{
						PlayerSessionID = Player.session_id;
						LocalPlayerTeamID = Player.team_id == "teamA" ? 0 : 1;// TODO
					}
				}
			}
		}
		break;
		case OPCODE_PLAYER_READY:
		{
			FNakameMsgPlayerReady PlayerReadyInfo;
			if (FJsonObjectConverter::JsonObjectStringToUStruct(MatchData.Data, &PlayerReadyInfo))
			{
				UE_LOG(LogTemp, Log, TEXT("On Received Match Data: Player Ready %s"), *PlayerReadyInfo.user_name);
				OnPlayerGetReadyDelegate.Broadcast(PlayerReadyInfo.user_name, true);
			}
		}
		break;
		case OPCODE_MATCH_READY:
		{
			FNakameMsgMatchReady MatchReadyInfo;
			if (FJsonObjectConverter::JsonObjectStringToUStruct(MatchData.Data, &MatchReadyInfo))
			{
				const FString LevelName = FString::Printf(TEXT("%s:%s"), *MatchReadyInfo.server_addr, *MatchReadyInfo.server_port);
				// game mode will parse login options and set name
				//FString Options = FString::Printf(TEXT("session_id=%s Name=%s"), *PlayerSessionID, *NakamaSession->SessionData.Username);
				APlayerController* LocalPlayerController = GetWorld()->GetFirstPlayerController();
				LocalPlayerController->SetInputMode(FInputModeGameOnly());
				FString Options = FString::Printf(TEXT("session_id=%s?custom_name=%s"), *PlayerSessionID, *PlayerDisplayName);
				UGameplayStatics::OpenLevel(this, FName(*LevelName), false, Options);
			}
		}
		break;
		case OPCODE_PLAYER_CHANGE_TEAM:
		{
			FNakameMsgChangeTeam PlayerChangeTeamInfo;
			if (FJsonObjectConverter::JsonObjectStringToUStruct(MatchData.Data, &PlayerChangeTeamInfo))
			{
				PlayerChangeTeamDelegate.Broadcast(PlayerChangeTeamInfo.user_name, PlayerChangeTeamInfo.old_team, PlayerChangeTeamInfo.new_team);
				if (PlayerChangeTeamInfo.user_id == NakamaSession->SessionData.UserId)
				{
					LocalPlayerTeamID = PlayerChangeTeamInfo.new_team;
				}
			}
		}
		break;
		default:
			UE_LOG(LogTemp, Log, TEXT("On Received Match Data: %s"), *MatchData.Data);
	}
}

void UPFNakamaSubsystem::OnReceivedMatchPresence(const FNakamaMatchPresenceEvent& PresenceEvent)
{
	for (int16 i = 0; i < PresenceEvent.Joins.Num(); i++)
	{
		FNakamaUserPresence Presence = PresenceEvent.Joins[i];
		UE_LOG(LogTemp, Log, TEXT("Player %s join in match"), *Presence.Username);
	}

	TArray<FString> LeavingPlayers;
	for (int16 i = 0; i < PresenceEvent.Leaves.Num(); i++)
	{
		FNakamaUserPresence Presence = PresenceEvent.Leaves[i];
		UE_LOG(LogTemp, Log, TEXT("Player %s leave match"), *Presence.Username);
		LeavingPlayers.Add(Presence.Username);
	}

	if (LeavingPlayers.Num() > 0)
	{
		PlayersLeaveMatchDelegate.Broadcast(LeavingPlayers);
	}
}

void UPFNakamaSubsystem::OnReceivedNotification(const FNakamaNotificationList& NotificationList)
{

	for (int16 i = 0; i < NotificationList.Notifications.Num(); i++)
	{
		FNakamaNotification Notification = NotificationList.Notifications[i];
		UE_LOG(LogTemp, Log, TEXT("Other notification: %s\n%s"), *Notification.Subject, *Notification.Content);
		//switch (Notification.Code)
		//{
		//	default:
		//		UE_LOG(LogTemp, Log, TEXT("Other notification: %s\n%s"), *Notification.Subject, *Notification.Content);
		//		break;
		//}
	}
}

bool UPFNakamaSubsystem::SettingUpValidationCheck()
{
	for (int Slot = MIN_WEAPON_SLOT; Slot <= MAX_WEAPON_SLOT-1; ++Slot)
	{
		FString Key = FString::Printf(TEXT("weapon_%d"), Slot);
		if(LocalPlayerInfo.weapons[Key] == EWeaponType::WT_None) return false;
	}

	for (int Slot = MIN_ABILITY_SLOT; Slot <= MAX_ABILITY_SLOT; ++Slot)
	{
		FString Key = FString::Printf(TEXT("skill_%d"), Slot);
		if (LocalPlayerInfo.skills[Key] == EAbilityType::AT_None) return false;
	}

	for (int Slot = MIN_PERK_SLOT; Slot <= MAX_PERK_SLOT; ++Slot)
	{
		FString Key = FString::Printf(TEXT("perk_%d"), Slot);
		if (LocalPlayerInfo.perks[Key] == EPerkType::PT_None) return false;
	}
	return true;
}

FString UPFNakamaSubsystem::GetPlayerDisplayName()
{
	return PlayerDisplayName;
}

void UPFNakamaSubsystem::RequestChangeAccountName(FString Username)
{
	if (NakamaSession)
	{ 
		PlayerDisplayName = Username;
		//FOnUpdateAccount UpdateAccountSuccessDelegate;
		//NakamaClient->UpdateAccount(NakamaSession, NakamaDeviceId, PlayerDisplayName, "", "en", "", "", UpdateAccountSuccessDelegate, NakamaClientErrorDelegate);
	}
}

void UPFNakamaSubsystem::RequestAddMatchmaker(EMatchMode MatchMode)
{
	LocalPlayerInfo.ResetToDefault();

	int32 MinPlayers = GetMinPlayers(MatchMode);
	int32 MaxPlayers = GetMinPlayers(MatchMode);
	FString Query = TEXT("");
	TMap<FString, FString> StringProperties = { { "mode", "authoritative" } };
	TMap<FString, int32> NumericProperties = { { "minPlayers", MinPlayers }, { "maxPlayers", MaxPlayers } };
	int32 CountMultiple = 0;
	bool IgnoreCountMultiple = true;

	if(NakamaRealtimeClient)
		NakamaRealtimeClient->AddMatchmaker(MinPlayers, MaxPlayers, Query, StringProperties, NumericProperties, CountMultiple, IgnoreCountMultiple, AddMatchmakerSuccessDelegate, NakamaRealtimeErrorDelegate);
	// TODO Popup Error Message
}

void UPFNakamaSubsystem::RequestCreateMatch(int32 MinPlayers, int32 MaxPlayers)
{
	LocalPlayerInfo.ResetToDefault();

	if(NakamaRealtimeClient)
	{ 
		// NakamaRealtimeClient->CreateMatch(CreateMatchSuccessDelegate, NakamaRealtimeErrorDelegate);

		MinPlayers = 2;// TODO
		FString HttpRequestPayload = FString::Printf(TEXT("{\"min_players\":%d, \"max_players\":%d}"), MinPlayers, MaxPlayers);
		FHttpRequestCompleteDelegate HttpRequestCompleteDelegate;
		HttpRequestCompleteDelegate.BindUObject(this, &UPFNakamaSubsystem::OnHttpCreateMatchResponse);

		FString RequestURL = FString::Printf(TEXT("http://%s:7350/v2/rpc/server_rpc.create_authoritative_match?http_key=defaulthttpkey&unwrap"), *NakamaHost);

		UPFGameplayFunctionLibrary::SendHttpRequestPost(RequestURL, HttpRequestPayload, HttpRequestCompleteDelegate);
	}
}

void UPFNakamaSubsystem::RequestJoinMatch(const FString& MatchID)
{
	if(NakamaRealtimeClient)
	{ 
		if (!NakamaMatchID.IsEmpty()) // we set NakamaMatchID after join match success
		{
			UE_LOG(LogTemp, Log, TEXT("RequestJoinMatch failed, already in match: %s."), *NakamaMatchID);
			return;
		}
		TMap<FString, FString> Metadata;
		NakamaRealtimeClient->JoinMatch(MatchID, Metadata, JoinMatchSuccessDelegate, NakamaRealtimeErrorDelegate);
	}
}

void UPFNakamaSubsystem::RequestLeaveMatch()
{
	if (NakamaRealtimeClient && !NakamaMatchID.IsEmpty())
	{
		NakamaRealtimeClient->LeaveMatch(NakamaMatchID, LeaveMatchSuccessDelegate, NakamaRealtimeErrorDelegate);
		NakamaMatchID.Empty();
		NakamaTicket.Empty();
	}
}

void UPFNakamaSubsystem::RequestLeaveMatchmaker()
{
	if (NakamaRealtimeClient && !NakamaTicket.IsEmpty())
	{
		NakamaRealtimeClient->LeaveMatchmaker(NakamaTicket, LeaveMatchmakerSuccessDelegate, NakamaRealtimeErrorDelegate);

		NakamaTicket.Empty();
	}
}

bool UPFNakamaSubsystem::RequestPlayerReady()
{
	if (NakamaRealtimeClient && SettingUpValidationCheck())
	{ 
		FNakameMsgPlayerReady Message;
		Message.user_id = NakamaSession->SessionData.UserId;
		FString JsonMsg;
		FJsonObjectConverter::UStructToJsonObjectString(Message, JsonMsg);
		NakamaRealtimeClient->SendMatchData(CurrentMatch.MatchId, OPCODE_PLAYER_READY, JsonMsg, {});
		return true;
	}
	return false;
}

void UPFNakamaSubsystem::RequestChangeTeam(int32 NewTeamID)
{
	if (NakamaRealtimeClient)
	{
		if(NewTeamID == LocalPlayerTeamID)
			return;

		FString JsonMsg = FString::Printf(TEXT("{\"old_team_id\":%d, \"new_team_id\":%d}"), LocalPlayerTeamID, NewTeamID);

		NakamaRealtimeClient->SendMatchData(CurrentMatch.MatchId, OPCODE_PLAYER_CHANGE_TEAM, JsonMsg, {});
	}
}

bool UPFNakamaSubsystem::RequestListMatches()
{
	if (NakamaRealtimeClient && NakamaSession)
	{
		int32 MinPlayers = 1;
		int32 MaxPlayers = 6;

		int32 Limit = 10;
		bool Authoritative = true;
		FString Label = TEXT("");

		NakamaClient->ListMatches(NakamaSession, MinPlayers, MaxPlayers, Limit, Label, Authoritative, ListMatchesSuccessDelegate, NakamaClientErrorDelegate);
		return true;
	}

	return false;
}

void UPFNakamaSubsystem::RequestSetWeapon(int32 SlotID, EWeaponType WeaponType)
{
	if (NakamaRealtimeClient && SlotID >= MIN_WEAPON_SLOT && SlotID <= MAX_WEAPON_SLOT)
	{
		FString Key = FString::Printf(TEXT("weapon_%d"), SlotID);

		if (LocalPlayerInfo.weapons[Key] == WeaponType) return;
		FString JsonMsg = FString::Printf(TEXT("{\"weapon_slot\":\"weapon_%d\", \"weapon_id\":%d}"), SlotID, int32(WeaponType));

		NakamaRealtimeClient->SendMatchData(CurrentMatch.MatchId, OPCODE_PLAYER_SET_WEAPON, JsonMsg, {});

		FString WeaponSlot = FString::Printf(TEXT("weapon_%d"), SlotID);
		LocalPlayerInfo.weapons[WeaponSlot] = EWeaponType(WeaponType);
	}
}

void UPFNakamaSubsystem::RequestSetSkill(int32 SlotID, EAbilityType SkillType)
{
	if (NakamaRealtimeClient && SlotID >= MIN_ABILITY_SLOT && SlotID <= MAX_ABILITY_SLOT)
	{
		FString Key = FString::Printf(TEXT("skill_%d"), SlotID);

		if(LocalPlayerInfo.skills[Key] == SkillType) return;
		FString JsonMsg = FString::Printf(TEXT("{\"skill_slot\":\"skill_%d\", \"skill_id\":%d}"), SlotID, int32(SkillType));

		NakamaRealtimeClient->SendMatchData(CurrentMatch.MatchId, OPCODE_PLAYER_SET_SKILL, JsonMsg, {});

		FString SkillSlot = FString::Printf(TEXT("skill_%d"), SlotID);
		LocalPlayerInfo.skills[SkillSlot] = EAbilityType(SkillType);
	}
}

void UPFNakamaSubsystem::RequestSetPerk(int32 SlotID, EPerkType PerkType)
{
	if (NakamaRealtimeClient && SlotID >= MIN_PERK_SLOT && SlotID <= MAX_PERK_SLOT)
	{
		FString Key = FString::Printf(TEXT("perk_%d"), SlotID);

		if (LocalPlayerInfo.perks[Key] == PerkType) return;
		FString JsonMsg = FString::Printf(TEXT("{\"perk_slot\":\"perk_%d\", \"perk_id\":%d}"), SlotID, int32(PerkType));

		NakamaRealtimeClient->SendMatchData(CurrentMatch.MatchId, OPCODE_PLAYER_SET_PERK, JsonMsg, {});

		FString PerkSlot = FString::Printf(TEXT("perk_%d"), SlotID);
		LocalPlayerInfo.perks[PerkSlot] = EPerkType(PerkType);
	}
}

void UPFNakamaSubsystem::RequestVoteMap(int32 MapID)
{
	if (NakamaRealtimeClient)
	{
		FString JsonMsg = FString::Printf(TEXT("{\"map_id\":\"%d\"}"), MapID);

		NakamaRealtimeClient->SendMatchData(CurrentMatch.MatchId, OPCODE_PLAYER_VOTE_MAP, JsonMsg, {});
	}
}

bool UPFNakamaSubsystem::IsWeaponEquipped(EWeaponType WeaponType)
{
	for (int32 SlotID = MIN_WEAPON_SLOT; SlotID <= MAX_WEAPON_SLOT; ++SlotID)
	{
		FString Key = FString::Printf(TEXT("weapon_%d"), SlotID);
		if (LocalPlayerInfo.weapons[Key] == WeaponType) return true;
	}
	return false;
}

bool UPFNakamaSubsystem::IsAbilityEquipped(EAbilityType AbilityTyp)
{
	for (int32 SlotID = MIN_ABILITY_SLOT; SlotID <= MAX_ABILITY_SLOT; ++SlotID)
	{
		FString Key = FString::Printf(TEXT("skill_%d"), SlotID);
		if (LocalPlayerInfo.skills[Key] == AbilityTyp) return true;
	}
	return false;
}

bool UPFNakamaSubsystem::IsPerkEquipped(EPerkType PerkTyp)
{
	for (int32 SlotID = MIN_PERK_SLOT; SlotID <= MAX_PERK_SLOT; ++SlotID)
	{
		FString Key = FString::Printf(TEXT("perk_%d"), SlotID);
		if (LocalPlayerInfo.perks[Key] == PerkTyp) return true;
	}
	return false;
}

void UPFNakamaSubsystem::ShowNakamaError(const FInGameError& Error)
{
	if (PopupManager == nullptr)
	{
		ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
		if (LocalPlayer)
		{
			PopupManager = LocalPlayer->GetSubsystem<UPFPopupSubsystem>();
		}
	}

	if (PopupManager)
	{
		FInGameError ErrorData;
		ErrorData.Message = FString::Printf(TEXT("%s(%d)"), *Error.Message, Error.ErrorCode);
		PopupManager->ShowInGameError(ErrorData);
	}
}

void UPFNakamaSubsystem::SyncLocalPlayerWeaponSet() const
{
	if (NakamaRealtimeClient)
	{
		for(const auto& WeaponSet : LocalPlayerInfo.weapons)
		{ 
			FNakameMsgPlayerReady Message;
			Message.user_id = NakamaSession->SessionData.UserId;
			FString JsonMsg = FString::Printf(TEXT("{\"weapon_slot\":\"%s\", \"weapon_id\":%d}"), *WeaponSet.Key, int32(WeaponSet.Value));

			NakamaRealtimeClient->SendMatchData(CurrentMatch.MatchId, OPCODE_PLAYER_SET_WEAPON, JsonMsg, {});
		}
	}
}

void UPFNakamaSubsystem::SyncLocalPlayerAbilitySet() const
{
	if (NakamaRealtimeClient)
	{
		for(const auto& AbilitySet : LocalPlayerInfo.skills)
		{ 
			FNakameMsgPlayerReady Message;
			Message.user_id = NakamaSession->SessionData.UserId;
			FString JsonMsg = FString::Printf(TEXT("{\"skill_slot\":\"%s\", \"skill_id\":%d}"), *AbilitySet.Key, int32(AbilitySet.Value));

			NakamaRealtimeClient->SendMatchData(CurrentMatch.MatchId, OPCODE_PLAYER_SET_SKILL, JsonMsg, {});
		}
	}
}

int UPFNakamaSubsystem::GetMinPlayers(EMatchMode MatchMode)
{
	return 2;
	//int PlayersNum = 0;
	//switch (MatchMode)
	//{
	//case EMatchMode::MM_1V1:
	//	PlayersNum = 2;
	//	break;
	//case EMatchMode::MM_None:
	//case EMatchMode::MM_3V3:
	//default:
	//	PlayersNum = 6;
	//	break;
	//}
	//return PlayersNum;
}

bool UPFNakamaSubsystem::GetSteamAccountInfo()
{
#ifdef STEAM_ENABLE
	if (SteamAPI_Init())
	{
		uint64 Sid;
		CSteamID SteamID = SteamUser()->GetSteamID();
		Sid = SteamID.ConvertToUint64();

		if (Sid != 0)
		{ 
			NakamaDeviceId = FString::Printf(TEXT("%llu"), Sid);
			PlayerDisplayName = SteamFriends()->GetPersonaName();
		}
	}
#endif

	return false;
}
