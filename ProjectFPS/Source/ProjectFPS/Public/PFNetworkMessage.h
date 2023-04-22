#pragma once
#include "CoreMinimal.h"
#include "PFEnums.h"
#include "PFNetworkMessage.generated.h"

// Example
// [{"session_id":"d0565fae-4b2a-11ed-88ed-006100a0eb06","team_id":"teamB","team_position":1,"user_id":"a5738f35-1c79-4090-a736-7e6d8367deb1"}]
USTRUCT(BlueprintType)
struct FNakamaMsgJoin
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Nakama Server|Message", meta=(DisplayName = "Session ID"))
	FString session_id;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Nakama Server|Message", meta = (DisplayName = "User ID"))
	FString user_id;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Nakama Server|Message", meta = (DisplayName = "User Name"))
	FString username;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Nakama Server|Message", meta = (DisplayName = "Team ID"))
	FString team_id;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Nakama Server|Message", meta = (DisplayName = "Player Status"))
	int32 status;
};

USTRUCT(BlueprintType)
struct FNakameMsgPlayerReady
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Nakama Server|Message", meta = (DisplayName = "User ID"))
	FString user_id;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Nakama Server|Message", meta = (DisplayName = "User Name"))
	FString user_name;
};

USTRUCT(BlueprintType)
struct FNakameMsgMatchReady
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Nakama Server|Message", meta = (DisplayName = "Server IP"))
	FString server_addr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Nakama Server|Message", meta = (DisplayName = "Server Port"))
	FString server_port;
};

USTRUCT(BlueprintType)
struct FNakameMsgChangeTeam
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Nakama Server|Message", meta = (DisplayName = "User Name"))
	FString	user_id;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Nakama Server|Message", meta = (DisplayName = "User Name"))
	FString user_name;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Nakama Server|Message", meta = (DisplayName = "Old Team"))
	int32 old_team;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Nakama Server|Message", meta = (DisplayName = "New Team"))
	int32 new_team;
};

USTRUCT(BlueprintType)
struct FNakamaPlayerInfo
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Nakama|Message")
	int32 status;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Nakama|Message")
	TMap<FString, EAbilityType> skills;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Nakama|Message")
	TMap<FString, EWeaponType> weapons;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Nakama|Message")
	TMap<FString, EPerkType> perks;

	FNakamaPlayerInfo();

	void ResetToDefault();
};

USTRUCT(BlueprintType)
struct FNakamaPresence
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Nakama|Message")
	FString node;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Nakama|Message")
	int32 reason;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Nakama|Message")
	FString session_id;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Nakama|Message")
	FString user_id;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Nakama|Message")
	FString username;
};

USTRUCT(BlueprintType)
struct FNakamaMatchState
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Nakama|Message")
	bool debug;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Nakama|Message")
	int32 player_num;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Nakama|Message")
	TMap<FString, FNakamaPlayerInfo> players_info;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Nakama|Message")
	TMap<FString, FNakamaPresence> presences;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Nakama|Message")
	FString server_addr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Nakama|Message")
	FString server_port;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Nakama|Message")
	TArray<FString> teamA;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Nakama|Message")
	TArray<FString> teamB;
};