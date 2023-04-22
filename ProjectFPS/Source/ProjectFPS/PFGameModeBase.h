// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "PFPlayerController.h"
#include <GameFramework/PlayerStart.h>
#include "PFNetworkMessage.h"

#include "Runtime/Online/HTTP/Public/Interfaces/IHttpRequest.h"
#include "Runtime/Online/HTTP/Public/Interfaces/IHttpResponse.h"

#include "PFGameModeBase.generated.h"

/**
 *
 */
UCLASS()
class PROJECTFPS_API APFGameModeBase : public AGameMode
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TMap<FString, class APFPlayerController*> RegisteredPlayers;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "GamePlay")
	int32 CreditsPerKill;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "GamePlay")
	float MaxWaitingTime;

	UPROPERTY()
	float GameReadyLeftTime;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "GamePlay")
	float MatchRoundTime;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "GamePlay")
	float MinimumPlayers = 2;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "GamePlay")
	float MaxPlayers = 4;

	UPROPERTY()
	float MatchRoundLeftTime;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "GamePlay")
	TArray<class APlayerStart*> TeamAPlayerStartPoints;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "GamePlay")
	TArray<class APlayerStart*> TeamBPlayerStartPoints;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GamePlay")
	int32 TeamANum;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GamePlay")
	int32 TeamBNum;

	UFUNCTION()
	void RespawnPlayerElapsed(AController* Controller);

protected:
	virtual void SetMatchState(FName NewState) override;

	UFUNCTION(BlueprintCallable)
	APlayerStart* FindBestSpawnPointForPlayer(AController* Controller);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<EWeaponType>	DebugWeaponList;


public:

	void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

	void PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;

	virtual APlayerController* Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal, const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;

	void PostLogin(APlayerController* NewPlayer) override;

	virtual void Logout(AController* Exiting) override;

	void StartToLeaveMap() override;

	UFUNCTION(BlueprintCallable)
	void RegisterPlayer(const FString& PlayerUniqueID, class APFPlayerController* PlayerController);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void RequestSpawn(class APFPlayerController* PlayerController, int32 TeamID);

	void OnActorKilled(AActor* VictimActor, AActor* KillerActor);

private:
	UPROPERTY()
	FNakamaMatchState NakamaMatchState;

	UPROPERTY()
	FString NakamaMatchID;

public:
	virtual void Tick(float DeltaSeconds) override;

	virtual bool ReadyToStartMatch_Implementation() override;

	virtual bool ReadyToEndMatch_Implementation() override;

protected:
	// UFUNCTION()
	void OnGetMatchStateCompleted(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void StartSpawnPlayer(APlayerController* NewPlayer, int32 TeamID);
};
