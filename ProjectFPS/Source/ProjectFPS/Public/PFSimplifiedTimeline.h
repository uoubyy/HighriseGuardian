// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine.h"
#include "UObject/NoExportTypes.h"
#include "Curves/CurveFloat.h"
#include "Engine/EngineTypes.h"
#include "PFSimplifiedTimeline.generated.h"

/**
 *
 */


UENUM(BlueprintType)		//"BlueprintType" is essential to include
enum class ESimplifiedTimelineDirection : uint8
{
	Forward	UMETA(DisplayName = "Forward"),
	Backward UMETA(DisplayName = "Backward")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSimplifiedTimerDelegate, float, CurrentValue, ESimplifiedTimelineDirection, Direction);

UCLASS(BlueprintType)
class PROJECTFPS_API UPFSimplifiedTimeline : public UObject
{
	GENERATED_BODY()

private:


	float UpdateInterval = 0.01f;

	ESimplifiedTimelineDirection CurrentDirection = ESimplifiedTimelineDirection::Forward;

	FTimerHandle TimerUpdateHandle;

	UPROPERTY()
	UWorld* World = nullptr;

	bool bIsPlaying = false;

	float CurrTime = 0.0f;
	float CurveStartTime;
	float CurveEndTime;
	float CurrentValue;

	inline float CalValue(float time);

	void StartTimer();

	UFUNCTION(Category = "Simplified Timeline")
		void UpdateTimer();

public:


	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Simplified Timeline", Meta = (ExposeOnSpawn = true))
		float PlaybackFrequency = 100;

	//UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Simplified Timeline", Meta = (ExposeOnSpawn = true))
	//UCurveFloat* Curve;

	UPROPERTY(BlueprintAssignable, Category = "Simplified Timeline")
		FSimplifiedTimerDelegate OnTimerUpdate;

	UPROPERTY(BlueprintAssignable, Category = "Simplified Timeline")
		FSimplifiedTimerDelegate OnTimerFinished;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Simplified Timeline", Meta = (ExposeOnSpawn = true))
		float StartValue;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Simplified Timeline", Meta = (ExposeOnSpawn = true))
		float EndValue;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Simplified Timeline", Meta = (ExposeOnSpawn = true))
		float TimeLength;

	// Functions

	UFUNCTION(BlueprintCallable, Category = "Simplified Timeline", meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
		void Play(UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "Simplified Timeline")
		void Pause();

	UFUNCTION(BlueprintCallable, Category = "Simplified Timeline")
		void Unpause();

	UFUNCTION(BlueprintCallable, Category = "Simplified Timeline")
		void Stop();

	UFUNCTION(BlueprintCallable, Category = "Simplified Timeline")
		void Reverse();


};
