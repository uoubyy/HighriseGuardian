// Fill out your copyright notice in the Description page of Project Settings.


#include "PFSimplifiedTimeline.h"

void UPFSimplifiedTimeline::Play(UObject* WorldContextObject)
{

	CurrentDirection = ESimplifiedTimelineDirection::Forward;

	if (!bIsPlaying)
	{
		World = GEngine->GetWorldFromContextObjectChecked(WorldContextObject);

		/*if (World != nullptr && Curve != nullptr)*/
		if (World != nullptr)
			StartTimer();

		return;
	}
	else
	{
		switch (CurrentDirection)
		{
		case ESimplifiedTimelineDirection::Forward:
			Unpause();
			break;
		case ESimplifiedTimelineDirection::Backward:
			Pause();
			CurrentDirection = ESimplifiedTimelineDirection::Forward;
			Unpause();
			break;
		default:
			break;
		}
	}
}

void UPFSimplifiedTimeline::StartTimer()
{
	// setup start and finish time.
	//Curve->GetTimeRange(CurveStartTime, CurveEndTime);
	CurveStartTime = 0;
	CurveEndTime = TimeLength;

	switch (CurrentDirection)
	{
	case ESimplifiedTimelineDirection::Forward:
		CurrTime = CurveStartTime;
		break;
	case ESimplifiedTimelineDirection::Backward:
		CurrTime = CurveEndTime;
		break;
	default:
		break;
	}

	// setup update interval based on frequency
	UpdateInterval = 1.0 / PlaybackFrequency;

	World->GetTimerManager().ClearAllTimersForObject(this);
	World->GetTimerManager().SetTimer(TimerUpdateHandle, this, &UPFSimplifiedTimeline::UpdateTimer, UpdateInterval, true);


	bIsPlaying = true;
}

void UPFSimplifiedTimeline::Pause()
{
	if (bIsPlaying)
	{
		if (!World->GetTimerManager().IsTimerPaused(TimerUpdateHandle))
			World->GetTimerManager().PauseTimer(TimerUpdateHandle);
	}
}

void UPFSimplifiedTimeline::Unpause()
{
	if (bIsPlaying)
	{
		if (World->GetTimerManager().IsTimerPaused(TimerUpdateHandle))
			World->GetTimerManager().UnPauseTimer(TimerUpdateHandle);
	}
}

void UPFSimplifiedTimeline::Stop()
{
	if (bIsPlaying)
	{
		if (World != nullptr)
		{
			if (World->GetTimerManager().TimerExists(TimerUpdateHandle))
				World->GetTimerManager().ClearTimer(TimerUpdateHandle);

			bIsPlaying = false;
		}
	}
}

void UPFSimplifiedTimeline::Reverse()
{
	CurrentDirection = ESimplifiedTimelineDirection::Backward;
	if (!bIsPlaying)
	{
		/*if (World != nullptr && Curve != nullptr)*/
		if (World != nullptr)
		{
			CurrTime = CurveEndTime;
			StartTimer();
		}
	}
}

void UPFSimplifiedTimeline::UpdateTimer()
{
	if (!bIsPlaying)
		return;

	bool bHasFinished = false;

	switch (CurrentDirection)
	{
	case ESimplifiedTimelineDirection::Forward:

		CurrTime += UpdateInterval;
		if (CurrTime >= CurveEndTime)
		{
			CurrTime = CurveEndTime;
			bHasFinished = true;
		}

		break;
	case ESimplifiedTimelineDirection::Backward:

		CurrTime -= UpdateInterval;
		if (CurrTime <= CurveStartTime)
		{
			CurrTime = CurveStartTime;
			bHasFinished = true;
		}
		break;
	default:
		break;
	}

	//CurrentValue = Curve->GetFloatValue(CurrTime);
	CurrentValue = CalValue(CurrTime);
	OnTimerUpdate.Broadcast(CurrentValue, CurrentDirection);

	if (bHasFinished)
	{
		Stop();
		OnTimerFinished.Broadcast(CurrentValue, CurrentDirection);
	}
}

float UPFSimplifiedTimeline::CalValue(float time)
{
	return StartValue + (EndValue - StartValue) * (time / TimeLength);
}