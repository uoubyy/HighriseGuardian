// Copyright FPS Team. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PFEnums.h"
#include "PFInGameErrorWidget.generated.h"

USTRUCT(BlueprintType)
struct FPFUIState
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	bool ShowMouseCursor = false;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInGameErrorWidgetClosedDelegate);

/**
 * 
 */
UCLASS()
class PROJECTFPS_API UPFInGameErrorWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* ErrorTitle;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* ErrorMessage;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UButton* ConfirmButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UButton* CancelButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UButton* ExitButton;

protected:
	UFUNCTION()
	void OnConfirmButtonClicked();

	UFUNCTION()
	void OnCancelButtonClicked();

	UFUNCTION()
	void OnExitButtonClicked();

	UPROPERTY()
	FInGameError GameErrorInfo;

	UFUNCTION()
	void OnErrorWidgetClosed();

public:

	virtual void NativeOnInitialized() override;

	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable)
	void SetupInGameErrorWidget(const FInGameError& ErrorData);

	UPROPERTY(BlueprintAssignable)
	FOnInGameErrorWidgetClosedDelegate OnInGameErrorWidgetClosed;

private:
	
	FPFUIState PreviousUIState;
};
