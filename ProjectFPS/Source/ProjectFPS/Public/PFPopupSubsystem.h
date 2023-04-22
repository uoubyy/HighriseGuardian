// Copyright FPS Team. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "PFEnums.h"
#include "PFPopupSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTFPS_API UPFPopupSubsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()

public:
	UPFPopupSubsystem();

	virtual void Initialize(FSubsystemCollectionBase& Collection);

	UFUNCTION(BlueprintCallable, Category = "Popup")
	void ShowInGameError(const FInGameError& Error);
	
	UFUNCTION(BlueprintCallable)
	void OnInGameErrorWidgetClosed();

private:
	UPROPERTY()
	TArray<FInGameError> InGameErrorQueue;

	UPROPERTY()
	TSubclassOf<class UPFInGameErrorWidget> InGameErrorWidgetClass;
	
	UPROPERTY()
	class UPFInGameErrorWidget* InGameErrorWidget;

	void ShowInGameErrorInternal();
};
