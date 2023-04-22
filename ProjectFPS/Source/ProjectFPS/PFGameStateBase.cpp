// Fill out your copyright notice in the Description page of Project Settings.


#include "PFGameStateBase.h"
#include "GameFramework/GameMode.h"
#include "Net/UnrealNetwork.h"
#include "PFPlayerState.h"

void APFGameStateBase::OnRep_MatchState()
{
	Super::OnRep_MatchState();

	int CountDown = 0;
	OnMatchStateChange.Broadcast(MatchState, PreviousMatchState);//, CountDown);
}

void APFGameStateBase::OnRep_StageCountDownChange()
{
	if (MatchStageCountDown == 0)
	{
		TimerHandle_CountDown.Invalidate();
		return;
	}

	FTimerDelegate TimerDelegate;

	//Binding our Lambda expression
	TimerDelegate.BindLambda([&]()
		{
			MatchStageCountDown = MatchStageCountDown > 0 ? MatchStageCountDown - 1 : 0;
			if (MatchStageCountDown == 0)
			{
				TimerHandle_CountDown.Invalidate();
			}
		});

	GetWorldTimerManager().SetTimer(TimerHandle_CountDown, TimerDelegate, 1.0f, true);
	OnMatchStageCountDownChange.Broadcast(MatchStageCountDown);
	UE_LOG(LogTemp, Warning, TEXT("OnRep_StageCountDownChange %d"), MatchStageCountDown);
}

void APFGameStateBase::OnRep_MatchScore()
{
	OnMatchScoreChange.Broadcast(TeamAScore, TeamBScore);
}

void APFGameStateBase::ReportKillRecord_Implementation(FKillRecord KillRecord)
{
	if(HasAuthority()) return;

	while (CurrentKillRecordsCount >= MaxCachedKillRecords)
	{
		LatestKillRecords.Pop();
		CurrentKillRecordsCount--;
	}

	LatestKillRecords.Enqueue(KillRecord);
	CurrentKillRecordsCount++;

	OnKillRecordReportedDelegate.Broadcast(KillRecord);
}

void APFGameStateBase::SetMatchStageCountDown(int32 CountDown)
{
	MatchStageCountDown = CountDown;
}

void APFGameStateBase::AddScore(int32 TeamID, int Score)
{
	if (TeamID == 0)
		TeamAScore += Score;
	else
		TeamBScore += Score;
}

const FPFMatchResult APFGameStateBase::GetMatchResult()
{
	FPFMatchResult MatchResult;

	MatchResult.ScoreTeam1 = TeamAScore;
	MatchResult.ScoreTeam2 = TeamBScore;

	for (auto PlayerStatBase : PlayerArray)
	{
		APFPlayerState* PlayerStat = Cast<APFPlayerState>(PlayerStatBase);
		FPFScoreInfo Info(PlayerStat->GetPlayerName(), PlayerStat->GetKilledNum(), PlayerStat->GetDeaddNum());
		if (PlayerStat->GetTeamID() == 0)
			MatchResult.ScoreListTeam1.Add(Info);
		else
			MatchResult.ScoreListTeam2.Add(Info);
	}

	MatchResult.ScoreListTeam1.Sort([](const FPFScoreInfo& Left, const FPFScoreInfo& Right) { return Left.KilledNum > Right.KilledNum || Left.DeadNum <= Right.DeadNum; });
	MatchResult.ScoreListTeam2.Sort([](const FPFScoreInfo& Left, const FPFScoreInfo& Right) { return Left.KilledNum > Right.KilledNum || Left.DeadNum <= Right.DeadNum; });

	return MatchResult;
}

void APFGameStateBase::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APFGameStateBase, MatchStageCountDown);
	DOREPLIFETIME(APFGameStateBase, TeamAScore);
	DOREPLIFETIME(APFGameStateBase, TeamBScore);
}
