// Copyright FPS Team. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "NakamaClient.h"
#include "NakamaSession.h"
#include "NakamaError.h"

#include "PFEnums.h"
#include "PFNetworkMessage.h"

#include "Runtime/Online/HTTP/Public/Interfaces/IHttpRequest.h"
#include "Runtime/Online/HTTP/Public/Interfaces/IHttpResponse.h"

#include "PFNakamaSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayersJoinMatch, const TArray<FNakamaMsgJoin>&, NewPlayersInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPlayerGetReady, const FString&, PlayerID, bool, IsReady);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayersLeaveMatch, const TArray<FString>&, LeavingPlayersInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnPlayerChangeTeam, const FString&, PlayerName, int32, OldTeamID, int32, NewTeamID);

/**
 * 
 */
UCLASS(Config=Game)
class PROJECTFPS_API UPFNakamaSubsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()

public:

	virtual void Initialize(FSubsystemCollectionBase& Collection);

	virtual void Deinitialize();

	// Login
	UPROPERTY(BlueprintAssignable, Category = "Nakama Server")
	FOnAuthUpdate AuthenticationSuccessDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Nakama Server")
	FOnError NakamaClientErrorDelegate;

	// Realtime Client
	UPROPERTY(BlueprintAssignable, Category = "Nakama Server")
	FOnRealtimeClientConnected ConnectionSuccessDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Nakama Server")
	FOnRealtimeClientError ConnectionErrorDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Nakama Server")
	FOnDisconnected NakamaDisconnectedDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Nakama Server")
	FOnRtError NakamaRealtimeErrorDelegate;

	// Account
	UPROPERTY(BlueprintAssignable, Category = "Nakama Server")
	FOnUserAccountInfo GetAccountSuccessDelegate;

	// Match Maker
	UPROPERTY(BlueprintAssignable, Category = "Nakama Server")
	FOnMatchmakerTicket AddMatchmakerSuccessDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Nakama Server")
	FOnRemovedMatchmakerTicket LeaveMatchmakerSuccessDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Nakama Server")
	FOnReceivedMatchmakerMatched MatchmakerMatchedSuccessDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Nakama Server")
	FOnCreateMatch JoinMatchSuccessDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Nakama Server")
	FOnReceivedMatchPresenceCallback ReceivedMatchPresenceDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Nakama Server")
	FOnPlayersJoinMatch PlayersJoinMatchDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Nakama Server")
	FOnPlayersLeaveMatch PlayersLeaveMatchDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Nakama Server")
	FOnPlayerChangeTeam PlayerChangeTeamDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Nakama Server")
	FOnCreateMatch CreateMatchSuccessDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Nakama Server")
	FOnLeaveMatch LeaveMatchSuccessDelegate;

	// Match Data
	UPROPERTY(BlueprintAssignable, Category = "Nakama Server")
	FOnReceivedMatchData ReceivedMatchDataDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Nakama Server")
	FOnMatchlist ListMatchesSuccessDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Nakama Server")
	FOnPlayerGetReady OnPlayerGetReadyDelegate;

	// Notification
	UPROPERTY(BlueprintAssignable, Category = "Nakama Server")
	FOnReceivedNotification NotificationReceivedDelegate;

	UFUNCTION(BlueprintCallable, Category = "Nakama Server")
	void NakamaLogin();

	UFUNCTION(BlueprintCallable, Category = "Nakama Server")
	void NakamaCreateRealTimeConnection();

	UFUNCTION(BlueprintCallable, Category = "Nakama Server")
	const FNakamaPlayerInfo GetLocalPlayerInfo() const { return LocalPlayerInfo; }

	UFUNCTION(BlueprintCallable, Category = "Nakama Server")
	const FString GetMatchID() const { return NakamaMatchID; }

	UFUNCTION(BlueprintCallable, Category = "Nakama Server")
	const FString GetVersionType() const { return IsReleaseVersion ? "Release" : "Test"; }

	UFUNCTION(BlueprintCallable, Category = "Nakama Server")
	void DestroyClient();
	
private:
	UPROPERTY()
	UNakamaClient* NakamaClient = nullptr;

	UPROPERTY()
	UNakamaRealtimeClient* NakamaRealtimeClient = nullptr;

	UPROPERTY()
	UNakamaSession* NakamaSession = nullptr;

	// Nakama Config
	UPROPERTY(Config)
	FString NakamaHost;

	UPROPERTY(Config)
	int32 NakamaPort;

	UPROPERTY()
	bool IsReleaseVersion;

	UPROPERTY(Config)
	FString NakamaDeviceId;

	// Nakama User Info
	UPROPERTY()
	FString PlayerSessionID;

	UPROPERTY()
	FString PlayerDisplayName;

	UPROPERTY()
	FNakamaMatch CurrentMatch;

	UPROPERTY()
	FNakamaPlayerInfo LocalPlayerInfo;

	UPROPERTY()
	int32 LocalPlayerTeamID;

	void GetServerConfig();

	void AuthenticateDevice();

	UFUNCTION()
	void OnAuthenticationSuccess(UNakamaSession* LoginData);

	UFUNCTION()
	void OnNakamaClientError(const FNakamaError& Error);

	UFUNCTION()
	void OnRealtimeClientConnectionSuccess();

	UFUNCTION()
	void OnRealtimeClientConnectionError();

	UFUNCTION()
	void OnRealtimeClientDisconnected(const FNakamaDisconnectInfo& DisconnectInfo);

	// Account
	UFUNCTION()
	void OnGetAccountSuccess(const FNakamaAccount& AccountData);

	// Match Maker Relevant
	UFUNCTION()
	void OnAddMatchmakerSuccess(FString Ticket);

	UFUNCTION()
	void OnCreateMatchSuccess(FNakamaMatch Match);

	void OnHttpCreateMatchResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully);

	UPROPERTY()
	FString NakamaTicket;

	UPROPERTY()
	FString NakamaMatchID;

	UFUNCTION()
	void OnMatchmakerMatchedSuccess(const FNakamaMatchmakerMatched& Match);

	UFUNCTION()
	void OnJoinMatchSuccess(FNakamaMatch Match);

	UFUNCTION()
	void OnJoinMatchError(const FNakamaRtError& Error);

	UFUNCTION()
	void OnNakamaRealtimeError(const FNakamaRtError& Error);

	UFUNCTION()
	void OnListMatchesSuccess(const FNakamaMatchList& MatchList);

	// Match Data
	UFUNCTION()
	void OnReceivedMatchData(const FNakamaMatchData& MatchData);

	// Player Presence
	UFUNCTION()
	void OnReceivedMatchPresence(const FNakamaMatchPresenceEvent& PresenceEvent);

	// Notification
	UFUNCTION()
	void OnReceivedNotification(const FNakamaNotificationList& NotificationList);

	bool SettingUpValidationCheck();

public:
	
	UFUNCTION(BlueprintCallable, Category = "Nakama Server")
	FString GetPlayerDisplayName();
	
	// Account
	UFUNCTION(BlueprintCallable, Category = "Nakama Server")
	void RequestChangeAccountName(FString Username);

	// Match Maker
	UFUNCTION(BlueprintCallable, Category = "Nakama Server")
	void RequestAddMatchmaker(EMatchMode MatchMode);

	UFUNCTION(BlueprintCallable, Category = "Nakama Server")
	void RequestCreateMatch(int32 MinPlayers, int32 MaxPlayers);

	UFUNCTION(BlueprintCallable, Category = "Nakama Server")
	void RequestJoinMatch(const FString& MatchID);

	UFUNCTION(BlueprintCallable, Category = "Nakama Server")
	void RequestLeaveMatch();

	UFUNCTION(BlueprintCallable, Category = "Nakama Server")
	void RequestLeaveMatchmaker();

	UFUNCTION(BlueprintCallable, Category = "Nakama Server")
	bool RequestPlayerReady();

	UFUNCTION(BlueprintCallable, Category = "Nakama Server")
	void RequestChangeTeam(int32 NewTeamID);

	UFUNCTION(BlueprintCallable, Category = "Nakama Server")
	bool RequestListMatches();

	UFUNCTION(BlueprintCallable, Category = "Nakama Server")
	void RequestSetWeapon(int32 SlotID, EWeaponType WeaponType);

	UFUNCTION(BlueprintCallable, Category = "Nakama Server")
	void RequestSetSkill(int32 SlotID, EAbilityType SkillType);

	UFUNCTION(BlueprintCallable, Category = "Nakama Server")
	void RequestSetPerk(int32 SlotID, EPerkType PerkType);

	UFUNCTION(BlueprintCallable, Category = "Nakama Server")
	void RequestVoteMap(int32 MapID);

	UFUNCTION(BlueprintCallable, Category = "Nakama Server")
	const int32 GetLocalPlayerTeamID() const { return LocalPlayerTeamID; }

	UFUNCTION(BlueprintCallable)
	bool IsWeaponEquipped(EWeaponType WeaponType);

	UFUNCTION(BlueprintCallable)
	bool IsAbilityEquipped(EAbilityType AbilityTyp);

	UFUNCTION(BlueprintCallable)
	bool IsPerkEquipped(EPerkType PerkTyp);

private:
	class UPFPopupSubsystem* PopupManager = nullptr;
	
	UFUNCTION()
	void ShowNakamaError(const FInGameError& Error);

	UFUNCTION()
	void SyncLocalPlayerWeaponSet() const;

	UFUNCTION()
	void SyncLocalPlayerAbilitySet() const;

	int GetMinPlayers(EMatchMode MatchMode);

	bool GetSteamAccountInfo();
};
