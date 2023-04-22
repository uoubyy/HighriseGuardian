// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PFEnums.h"
#include "PFScoreboardWidget.generated.h"

/**
 *
 */
UCLASS()
class PROJECTFPS_API UPFScoreboardWidget : public UUserWidget
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class UDataTable* SkillsInfoDataTable;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class UDataTable* WeaponInfoDataTable;
public:
	UFUNCTION(BlueprintImplementableEvent)
	void InitScoreboard(const TArray<FPFScoreInfo>& ScoreList1, const TArray<FPFScoreInfo>& ScoreList2, const int32 TeamAScore, const int32 TeamBScore, const TArray<APFPlayerState*>& TeamInfo1, const TArray<APFPlayerState*>& TeamInfo2, const EMatchResult& MatchResult);

	UFUNCTION(BlueprintCallable)
	void ToggleScoreboard(bool Visible);

	UFUNCTION(BlueprintCallable)
	TArray<UTexture2D*> GetWeaponIcon(APFPlayerState* inPlayerState);

	UFUNCTION(BlueprintCallable)
	TArray<UTexture2D*> GetSkillIcon(APFPlayerState* inPlayerState);
};