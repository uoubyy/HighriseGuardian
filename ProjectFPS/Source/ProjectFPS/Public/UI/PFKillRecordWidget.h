// Copyright FPS Team. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PFEnums.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "PFKillRecordWidget.generated.h"

UENUM(BlueprintType)
enum class EWidgetStateType : uint8
{
	WST_Idle,
	WST_FadeIn,
	WST_Active,
	WST_FadeOut
};

/**
 * 
 */
UCLASS()
class PROJECTFPS_API UPFKillRecordWidget : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* CauserName;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UImage* WeaponIcon;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* VictimName;

	UPROPERTY(BlueprintReadOnly)
	EWidgetStateType WidgetState = EWidgetStateType::WST_Idle;

	UPROPERTY(EditDefaultsOnly)
	float MaxLifeTime;

	UPROPERTY(BlueprintReadOnly)
	float LifeTime;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess))
	float FadeOutDuration;

	UFUNCTION(BlueprintImplementableEvent)
	void OnFadeIn();

	UFUNCTION(BlueprintImplementableEvent)
	void OnFadeOut();

private:

	UPROPERTY(EditDefaultsOnly)
	class UDataTable* WeaponDataTable = nullptr;

	FTimerHandle ActiveTimerHandle;

	UFUNCTION()
	void OnDeActive();

public:
	
	UFUNCTION(BlueprintCallable)
	void InitKillRecordWidget(const FKillRecord& KillRecord);
};
