// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "Containers/Queue.h"
#include "PFPlayerController.h"
#include "PFEnums.h"
#include "PFGameStateBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMatchStateChange, FName, NewState, FName, OldState);//, int CountDown);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMatchStageCountDownChange, int32, CountDown);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMatchScoreChange, int32, TeamAScore, int32, TeamBScore);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnKillRecordReportedDelegate, FKillRecord, KillRecord);

/**
 *
 */
UCLASS()
class PROJECTFPS_API APFGameStateBase : public AGameState
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_StageCountDownChange)
	int32 MatchStageCountDown;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MatchScore)
	int32 TeamAScore;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MatchScore)
	int32 TeamBScore;

	TQueue<FKillRecord> LatestKillRecords;

	UPROPERTY(EditDefaultsOnly)
	int32 MaxCachedKillRecords = 5;

	int32 CurrentKillRecordsCount = 0;

public:

	virtual void OnRep_MatchState() override;

	UFUNCTION()
	void OnRep_StageCountDownChange();

	UFUNCTION()
	void SetMatchStageCountDown(int32 CountDown);

	UFUNCTION()
	int32 GetMatchStageCountDown() const { return MatchStageCountDown; }

	UFUNCTION()
	void OnRep_MatchScore();

	UFUNCTION(NetMulticast, Reliable, Category=GameState)
	void ReportKillRecord(FKillRecord KillRecord);

	FOnMatchStateChange OnMatchStateChange;

	FOnMatchStageCountDownChange OnMatchStageCountDownChange;

	FTimerHandle TimerHandle_CountDown;

	FOnMatchScoreChange OnMatchScoreChange;

	FOnKillRecordReportedDelegate OnKillRecordReportedDelegate;

	UFUNCTION(BlueprintCallable)
	void AddScore(int32 TeamID, int Score);

	UFUNCTION(BlueprintCallable)
	const FPFMatchResult GetMatchResult();

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const;
};
