// Copyright FPS Team. All Rights Reserved.


#include "PFPopupSubsystem.h"
#include "Blueprint/UserWidget.h"
#include "UI/PFInGameErrorWidget.h"

UPFPopupSubsystem::UPFPopupSubsystem() :ULocalPlayerSubsystem()
{
	static ConstructorHelpers::FClassFinder<UPFInGameErrorWidget>InGameErrorWidgetBP(TEXT("/Game/ProjectFPS/Core/UI/BP_InGameError"));
	if (InGameErrorWidgetBP.Class != nullptr)
	{
		InGameErrorWidgetClass = InGameErrorWidgetBP.Class;
	}
}

void UPFPopupSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

}

void UPFPopupSubsystem::ShowInGameError(const FInGameError& Error)
{
	InGameErrorQueue.Push(Error);
	ShowInGameErrorInternal();
}

void UPFPopupSubsystem::OnInGameErrorWidgetClosed()
{
	ShowInGameErrorInternal();
}

void UPFPopupSubsystem::ShowInGameErrorInternal()
{
	if (InGameErrorWidget == nullptr)
	{
		InGameErrorWidget = Cast<UPFInGameErrorWidget>(CreateWidget(GetWorld(), InGameErrorWidgetClass));
		InGameErrorWidget->OnInGameErrorWidgetClosed.AddDynamic(this, &UPFPopupSubsystem::OnInGameErrorWidgetClosed);
	}

	bool IsInViewport = InGameErrorWidget ? InGameErrorWidget->IsInViewport() : false;

	if (InGameErrorWidget && !InGameErrorQueue.IsEmpty() && !InGameErrorWidget->IsInViewport())
	{
		FInGameError Error = InGameErrorQueue.Pop();

		InGameErrorWidget->SetupInGameErrorWidget(Error);
		InGameErrorWidget->AddToViewport(100);
	}
}
