// Copyright FPS Team. All Rights Reserved.


#include "UI/PFKillRecordWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "DataTable/PFDataTableRowWeapon.h"
#include "Components/ListView.h"

void UPFKillRecordWidget::OnDeActive()
{
	GetWorld()->GetTimerManager().ClearTimer(ActiveTimerHandle);

	UListView* OwningListView = Cast<UListView>(GetOwningListView());
	if(!OwningListView) return;
	int32 RecordsNum = OwningListView->GetNumItems();

	// UE_LOG(LogTemp, Warning, TEXT("OnDeActive %s"), *CauserName->GetText().ToString());

	TArray<UPFKillRecordWidget*> NewListItems;
	for (auto Index = 0; Index < RecordsNum - 1; ++Index)
	{
		UPFKillRecordWidget* RecordItem = Cast<UPFKillRecordWidget>(OwningListView->GetItemAt(Index));
		if (RecordItem)
		{
			NewListItems.Add(RecordItem);
		}
	}
	OwningListView->SetListItems(NewListItems);
}

void UPFKillRecordWidget::InitKillRecordWidget(const FKillRecord& KillRecord)
{

	UE_LOG(LogTemp, Warning, TEXT("InitKillRecordWidget %s"), *KillRecord.CauserName);

	static const FString ContextString;

	if (WeaponDataTable)
	{
		FName RowName = UEnum::GetValueAsName(KillRecord.WeaponType);
		FPFDataTableRowWeapon* DataRow = WeaponDataTable->FindRow<FPFDataTableRowWeapon>(RowName, ContextString);
		if (DataRow)
		{
			CauserName->SetText(FText::FromString(KillRecord.CauserName));
			WeaponIcon->SetBrushFromTexture(DataRow->KillRecordWeaponIcon);
			VictimName->SetText(FText::FromString(KillRecord.VictimName));

			WidgetState = EWidgetStateType::WST_Active;

			LifeTime = MaxLifeTime;

			FTimerDelegate TimerDelegate;
			TimerDelegate.BindLambda([&]()
				{
					LifeTime -= FadeOutDuration;
					if (LifeTime <= FadeOutDuration && WidgetState == EWidgetStateType::WST_Active)
					{
						WidgetState = EWidgetStateType::WST_FadeOut;
						OnFadeOut();
					}
					else if (LifeTime <= 0.0f && WidgetState == EWidgetStateType::WST_FadeOut)
					{
						WidgetState = EWidgetStateType::WST_Idle;
						OnDeActive();
					}
				});

			GetWorld()->GetTimerManager().SetTimer(ActiveTimerHandle, TimerDelegate, FadeOutDuration, true);
		}
	}
}
