// Copyright FPS Team. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PFHitIndicatorWidget.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTFPS_API UPFHitIndicatorWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UBorder* HitIndicator;

	UPROPERTY(EditDefaultsOnly)
	float MaxVisibleTime = 5.0f;

	// virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UFUNCTION(BlueprintImplementableEvent)
	void FadeIn();

	UFUNCTION(BlueprintImplementableEvent)
	void FadeOut();
	
public:

	UFUNCTION(BlueprintCallable)
	void ActiveHitIndicator(/*AActor* Causer, AActor* Victim*/FRotator DamageDirection);

	UFUNCTION(BlueprintCallable)
	void DeActiveHitIndicator();

	UFUNCTION(BlueprintImplementableEvent)
	void InitPositionAndAlignment();

private:
	UPROPERTY()
	bool IsActive = false;

	FTimerHandle LifeTimerHandler;

	//float LifeTime;

	//UPROPERTY()
	//AActor* CauserActor;

	//UPROPERTY()
	//AActor* VictimActor;

};
