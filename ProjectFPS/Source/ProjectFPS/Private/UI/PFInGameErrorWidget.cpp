// Copyright FPS Team. All Rights Reserved.


#include "UI/PFInGameErrorWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

void UPFInGameErrorWidget::OnConfirmButtonClicked()
{
	this->RemoveFromParent();
	GameErrorInfo.OnPopupConfirmDelegate.Broadcast();
	OnErrorWidgetClosed();
}

void UPFInGameErrorWidget::OnCancelButtonClicked()
{
	this->RemoveFromParent();
	GameErrorInfo.OnPopupCancelDelegate.Broadcast();
	OnErrorWidgetClosed();
}

void UPFInGameErrorWidget::OnExitButtonClicked()
{
	this->RemoveFromParent();
	OnErrorWidgetClosed();
}

void UPFInGameErrorWidget::OnErrorWidgetClosed()
{
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if(PreviousUIState.ShowMouseCursor)
		PlayerController->SetInputMode(FInputModeGameAndUI());
	else
		PlayerController->SetInputMode(FInputModeGameOnly());

	PlayerController->SetShowMouseCursor(PreviousUIState.ShowMouseCursor);

	OnInGameErrorWidgetClosed.Broadcast();
}

void UPFInGameErrorWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	ConfirmButton->OnClicked.AddDynamic(this, &UPFInGameErrorWidget::OnConfirmButtonClicked);
	CancelButton->OnClicked.AddDynamic(this, &UPFInGameErrorWidget::OnCancelButtonClicked);
	ExitButton->OnClicked.AddDynamic(this, &UPFInGameErrorWidget::OnExitButtonClicked);
}

void UPFInGameErrorWidget::NativeConstruct()
{
	Super::NativeConstruct();

	//ConfirmButton->OnClicked.AddDynamic(this, &UPFInGameErrorWidget::OnConfirmButtonClicked);
	//CancelButton->OnClicked.AddDynamic(this, &UPFInGameErrorWidget::OnCancelButtonClicked);
	//ExitButton->OnClicked.AddDynamic(this, &UPFInGameErrorWidget::OnExitButtonClicked);
}

void UPFInGameErrorWidget::SetupInGameErrorWidget(const FInGameError& ErrorData)
{
	GameErrorInfo = ErrorData;
	ErrorMessage->SetText(FText::FromString(ErrorData.Message));

	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();

	PreviousUIState.ShowMouseCursor = PlayerController->ShouldShowMouseCursor();

	FInputModeUIOnly InputMode;
	InputMode.SetWidgetToFocus(this->GetCachedWidget());

	PlayerController->SetInputMode(InputMode);
	PlayerController->SetShowMouseCursor(true);
}
