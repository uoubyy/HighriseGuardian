// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/PFInGameHUD.h"
#include "PFAttributeComponent.h"
#include "Components/Button.h"
#include "Components/ProgressBar.h"
#include <Components/Border.h>
#include <Components/Image.h>
#include <Components/ListView.h>
#include <Components/CanvasPanel.h>
#include "Kismet/GameplayStatics.h"
#include "PFGameConstant.h"
#include "DataTable/PFDataTableRowSkill.h"
#include "DataTable/PFDataTableRowWeapon.h"
#include "UI/PFKillRecordWidget.h"
#include "UI/PFHitIndicatorWidget.h"
#include "PFGameplayFunctionLibrary.h"
#include "UI/PFPickupIndicatorWidget.h"
#include "PFPickUpBase.h"
#include "PFPlayerState.h"
#include <Components/TextBlock.h>
#include <string>
#include "Kismet/KismetMathLibrary.h"

void UPFInGameHUD::NativeConstruct()
{
	Super::NativeConstruct();

	DebugButton->SetVisibility(ESlateVisibility::Collapsed);
	
#if WITH_EDITOR
	DebugButton->SetVisibility(ESlateVisibility::Visible);
	DebugButton->OnClicked.AddDynamic(this, &UPFInGameHUD::OpenDebugLevel);
#endif
}

void UPFInGameHUD::InitInGameHUD(UPFAttributeComponent* AttributeComp)
{
#if WITH_EDITOR
	if(AttributeComp->GetNetMode() == ENetMode::NM_Client)
		DebugButton->SetVisibility(ESlateVisibility::Collapsed);
#endif

	AttributeComp->OnHealthChangedDelegate.AddDynamic(this, &UPFInGameHUD::OnHealthChanged);
	AttributeComp->OnCauseDamageDelegate.AddDynamic(this, &UPFInGameHUD::OnCauseDamage);
	OnHealthChanged(nullptr, AttributeComp, AttributeComp->GetHealthMax(), 0.0f);
}

void UPFInGameHUD::RefreshInGameHUD(APFPlayerState* PlayerState)
{
	if (PlayerState)
	{
		UserName = PlayerState->GetPlayerName();
		WeaponList = PlayerState->GetInventoryWeapons();
		AbilityList = PlayerState->GetInventoryAbilities();

		UE_LOG(LogTemp, Warning, TEXT("Weapon Num %d, Ability Num %d."), WeaponList.Num(), AbilityList.Num());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("RefreshInGameHUD: No PlayerState found!"));
		return;
	}

	static const FString ContextString;

	//Init Skill Panel
	if (SkillsInfoDataTable && PlayerState)
	{
		for (int32 SlotID = 1; SlotID <= MAX_ABILITY_SLOT; ++SlotID)
		{
			FString KeyName = FString::Printf(TEXT("skill_%d"), SlotID);
			FName RowName = UEnum::GetValueAsName(PlayerState->GetAbilityTypeOfSlot(SlotID));
			FPFDataTableRowSkill* DataRow = SkillsInfoDataTable->FindRow<FPFDataTableRowSkill>(RowName, ContextString);
			if (DataRow)
			{
				UTexture2D* Icon = DataRow->AbilityIcon;

				UImage* SlotIcon = SlotID == 1 ? SkillIcon1 : SkillIcon2;
				SlotIcon->SetBrushFromTexture(Icon);
			}
		}
	}

	//Init Weapon Panel
	int* ammoAmountList[MAX_WEAPON_SLOT] = { &AmmoAmount1, &AmmoAmount2,  &AmmoAmount3,  &AmmoAmount4 };
	int* maxAmmoAmountList[MAX_WEAPON_SLOT] = { &MaxAmmoAmount1,&MaxAmmoAmount2, &MaxAmmoAmount3, &MaxAmmoAmount4 };
	if (WeaponInfoDataTable && PlayerState) {
		for (int i = 1; i <= MAX_WEAPON_SLOT; i++) {
			EWeaponType weaponType = PlayerState->GetWeaponTypeOfSlot(i);
			FName weaponName = UEnum::GetValueAsName(PlayerState->GetWeaponTypeOfSlot(i));
			FPFDataTableRowWeapon* DataRow = WeaponInfoDataTable->FindRow<FPFDataTableRowWeapon>(weaponName, ContextString);
			
			if (DataRow)
			{
				UTexture2D* Icon = DataRow->KillRecordWeaponIcon;
				int ammo = DataRow->maxAmmo;
				
				*ammoAmountList[i-1] = ammo;
				*maxAmmoAmountList[i-1] = ammo;
				switch (i) {
				case 1:WeaponIcon1->SetBrushFromTexture(Icon);  break;
				case 2:WeaponIcon2->SetBrushFromTexture(Icon);  break;
				case 3:WeaponIcon3->SetBrushFromTexture(Icon);  break;
				case 4:WeaponIcon4->SetBrushFromTexture(Icon);  break;
				}
			}
		}
	}


	
}

void UPFInGameHUD::SetWeaponSlot(EWeaponType Weapon) {
	UImage* weaponIconList[MAX_WEAPON_SLOT] = { WeaponIcon1,WeaponIcon2, WeaponIcon3, WeaponIcon4 };
	UImage* weaponIconBGList[MAX_WEAPON_SLOT] = { WeaponBg_1,WeaponBg_2, WeaponBg_3, WeaponBg_4 };
	// TODO: change into dynamic list
	for (int i = 0; i < WeaponList.Num(); i++) {
		if (WeaponList[i] == Weapon) {
			weaponIconList[i]->SetDesiredSizeOverride(FVector2D(208, 105.6));
			weaponIconBGList[i]->SetDesiredSizeOverride(FVector2D(442, 130));
			weaponIconList[i]->SetColorAndOpacity(FLinearColor(1, 1, 1, 1));
			weaponIconBGList[i]->SetColorAndOpacity(FLinearColor(1, 1, 1, 1));
		}
		else {
			weaponIconList[i]->SetDesiredSizeOverride(FVector2D(166.4, 84.48));
			weaponIconBGList[i]->SetDesiredSizeOverride(FVector2D(353.6, 104));
			weaponIconList[i]->SetColorAndOpacity(FLinearColor(1, 1, 1, 0.5));
			weaponIconBGList[i]->SetColorAndOpacity(FLinearColor(1, 1, 1, 0.5));
		}
	}

}

int UPFInGameHUD::GetAbilitySlot(EAbilityType ability) {
	
	// TODO: change into dynamic list
	for (int i = 0; i < MAX_ABILITY_SLOT; i++) {
		if (AbilityList[i] == ability) {
			return i;
		}
	}
	return -1;

}

void UPFInGameHUD::OnDeadNumChanged(APFPlayerState* PlayerState, int32 NewNum, int32 Delta)
{
	DeadNum = NewNum;
}

void UPFInGameHUD::OnKilledNumChanged(APFPlayerState* PlayerState, int32 NewNum, int32 Delta)
{
	KilledNum = NewNum;
}

void UPFInGameHUD::OnCreditsChanged(APFPlayerState* PlayerState, int32 NewCredits, int32 Delta)
{
	Credits = NewCredits;
}

void UPFInGameHUD::OnAmmoAmountChanged(int AmmoAmount)
{

	UTextBlock* ammoList[MAX_WEAPON_SLOT] = { Ammo1, Ammo2,  Ammo3,  Ammo4 };
	UTextBlock* maxAmmoList[MAX_WEAPON_SLOT] = { MaxAmmo1,MaxAmmo2, MaxAmmo3, MaxAmmo4 };
	
	int* ammoAmountList[MAX_WEAPON_SLOT] = { &AmmoAmount1, &AmmoAmount2,  &AmmoAmount3,  &AmmoAmount4 };
	int* maxAmmoAmountList[MAX_WEAPON_SLOT] = { &MaxAmmoAmount1,&MaxAmmoAmount2, &MaxAmmoAmount3, &MaxAmmoAmount4 };

	int Ammo = 0;
	int MaxAmmo = 0;

	for (int i = 0; i < WeaponList.Num(); i++) {
		if (WeaponList[i] == CurrentWeapon) {
			
			Ammo = *ammoAmountList[i];
			MaxAmmo = *maxAmmoAmountList[i];
			//ammoList[i]->SetText(FText::FromString(FString::FromInt(AmmoAmount)));
			
			FSlateFontInfo font = ammoList[i]->Font;
			font.Size = 45;
			ammoList[i]->SetFont(font);
			*ammoAmountList[i] = AmmoAmount;

			font = maxAmmoList[i]->Font;
			font.Size = 25;
			maxAmmoList[i]->SetFont(font);

			int LowAmmoThreshold = FMath::CeilToInt(0.2f * MaxAmmo);
			if (Ammo > LowAmmoThreshold && AmmoAmount <= LowAmmoThreshold)
			{
				TogglePickupIndicators(EPowerupType::PT_AmmoBox, true);
			}
			else if (Ammo <= LowAmmoThreshold && AmmoAmount > LowAmmoThreshold)
			{
				TogglePickupIndicators(EPowerupType::PT_AmmoBox, false);
			}
		}
		else {
			FSlateFontInfo font = ammoList[i]->Font;
			font.Size = 35;
			ammoList[i]->SetFont(font);
			
			font = maxAmmoList[i]->Font;
			font.Size = 18;
			maxAmmoList[i]->SetFont(font);
		}
	}
	
	
	OnAmmoAmountChangedBP(AmmoAmount,MaxAmmo);
}

void UPFInGameHUD::OnMaxAmmoChanged(int MaxAmmoAmount)
{

}

void UPFInGameHUD::OnHealthChanged(AActor* InstigatorActor, UPFAttributeComponent* OwningComp, float NewValue, float Delta)
{
	int LowHealthThreshold = FMath::CeilToInt(0.2f * OwningComp->GetHealthMax());

	HealthPercent = FMath::CeilToInt(NewValue);
	int CurrentHealth = FMath::CeilToInt(HealthBar->Percent * OwningComp->GetHealthMax());

	if (CurrentHealth > LowHealthThreshold && HealthPercent <= LowHealthThreshold)
	{
		TogglePickupIndicators(EPowerupType::PT_HealthBox, true);
	}
	else if (CurrentHealth <= LowHealthThreshold && HealthPercent > LowHealthThreshold)
	{
		TogglePickupIndicators(EPowerupType::PT_HealthBox, false);
	}

	HealthBar->SetPercent(HealthPercent / OwningComp->GetHealthMax());

	
	if (Delta < 0.0f && InstigatorActor)
	{
		FString InstigatorName = InstigatorActor->GetName();
		UPFHitIndicatorWidget* HitIndicator = HitIndicators.Contains(InstigatorName) ?  HitIndicators[InstigatorName] : nullptr;
		if (!HitIndicator)
		{
			HitIndicator = CreateWidget<UPFHitIndicatorWidget>(this, HitIndicatorWidgetClass);
			HitIndicatorCanvas->AddChild(HitIndicator);

			HitIndicators.Add(InstigatorName, HitIndicator);
		}

		HitIndicator->InitPositionAndAlignment();

		AActor* VictimActor = OwningComp->GetOwner();

		FRotator RelevantRot = UKismetMathLibrary::FindLookAtRotation(VictimActor->GetActorLocation(), InstigatorActor->GetActorLocation());
		FRotator VictimRot = VictimActor->GetActorRotation();
		HitIndicator->ActiveHitIndicator(RelevantRot - VictimRot);
	}
}

void UPFInGameHUD::OnCauseDamage(AActor* VictimActor, float NewHealth, float Delta)
{
	SpawnHurtNum(-Delta);
	ShowHitmarker();
	if (NewHealth <= 0)
		ShowKillIcon();
}

void UPFInGameHUD::OnWeaponChanged_Implementation(EWeaponType NewWeapon, EWeaponType OldWeapon)
{
	TogglePickupIndicators(EPowerupType::PT_AmmoBox, false);
	CurrentWeapon = NewWeapon;
	SetWeaponSlot(NewWeapon);
}

void UPFInGameHUD::TogglePickupIndicators(EPowerupType PickupType, bool Value)
{
	if (PickupActors.IsEmpty())
	{
		PickupActors = UPFGameplayFunctionLibrary::GetAllPickups(GetWorld());

		for (auto Actor : PickupActors)
		{
			APFPickUpBase* Pickup = Actor.IsValid() ? Cast<APFPickUpBase>(Actor.Get()) : nullptr;
			if (Pickup)
			{
				UPFPickupIndicatorWidget* PickupIndicator = CreateWidget<UPFPickupIndicatorWidget>(this, PickupIndicatorWidgetClass);
				PickupIndicator->InitPickupIndicator(Pickup, Pickup->GetPowerupType());

				PickupIndicators.Add(PickupIndicator);
				RootPanel->AddChild(PickupIndicator);
				PickupIndicator->SetVisibility(ESlateVisibility::Hidden);
			}
		}
	}

	if (Value == false)
	{
		for ( auto Indicator : PickupIndicators )
		{
			Indicator->TogglePickupIndicator(false);
		}
	}
	else
	{
		for ( auto Indicator : PickupIndicators )
		{
			bool IndicatorVisibility = Indicator->GetPickupIndicatorTargetType() == PickupType && (PickupType == EPowerupType::PT_HealthBox || Indicator->GetPickupIndicatorTargetWeaponType() == CurrentWeapon);
			Indicator->TogglePickupIndicator(IndicatorVisibility);
		}
	}
}

void UPFInGameHUD::OpenDebugLevel()
{
#if !UE_BUILD_SHIPPING
	UGameplayStatics::OpenLevel(this, FName("127.0.0.1:17777"), false);
	DebugButton->SetVisibility(ESlateVisibility::Hidden);
#endif
}
