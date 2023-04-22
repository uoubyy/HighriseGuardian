// Copyright FPS Team. All Rights Reserved.


#include "UI/PFInLobbyHUD.h"
#include "UI/PFAbilitySlotWidget.h"
#include "UI/PFWeaponSlotWidget.h"
#include "UI/PFPerkSlotWidget.h"
#include "Styling/SlateBrush.h"
#include "Styling/SlateStyle.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

#include "DataTable/PFDataTableRowWeapon.h"
#include "DataTable/PFDataTableRowSkill.h"
#include "DataTable/PFDataTableRowPerk.h"

#include "PFGameConstant.h"
#include "Internationalization/Internationalization.h"

void UPFInLobbyHUD::NativeConstruct()
{
	Super::NativeConstruct();
}

void UPFInLobbyHUD::InitLobbyHUD(const FNakamaPlayerInfo& PlayerInfo, const FString& NakamaMatchID)
{
	FString MatchIDStr = FString::Printf(TEXT("Match ID: %s"), *NakamaMatchID);
	MatchID->Text = FText::FromString(MatchIDStr);

	mapID = 0;
	SetMapOverview();

	static const FString ContextString;
	if (WeaponsInfoDataTable)
	{
		for (int32 SlotID = 1; SlotID <= MAX_WEAPON_SLOT-1; ++SlotID)
		{
			FString KeyName = FString::Printf(TEXT("weapon_%d"), SlotID);
			FName RowName = UEnum::GetValueAsName(PlayerInfo.weapons[KeyName]);
			FPFDataTableRowWeapon* DataRow = WeaponsInfoDataTable->FindRow<FPFDataTableRowWeapon>(RowName, ContextString);
			if (DataRow)
			{
				WeaponSlots[SlotID - 1]->InitSlot(DataRow->WeaponType, DataRow->WeaponIcon);
				WeaponSlots[SlotID - 1]->SlotID = SlotID;
			}

		}
	}

	if (SkillsInfoDataTable)
	{
		for (int32 SlotID = 1; SlotID <= MAX_ABILITY_SLOT; ++SlotID)
		{
			FString KeyName = FString::Printf(TEXT("skill_%d"), SlotID);
			FName RowName = UEnum::GetValueAsName(PlayerInfo.skills[KeyName]);
			FPFDataTableRowSkill* DataRow = SkillsInfoDataTable->FindRow<FPFDataTableRowSkill>(RowName, ContextString);
			if(DataRow)
			{ 
				AbilitySlots[SlotID - 1]->InitSlot(DataRow->AbilityType, DataRow->AbilityIcon);
				AbilitySlots[SlotID - 1]->SlotID = SlotID;
			}
		}
	}

	if (PerksInfoDataTable)
	{
		for (int32 SlotID = 1; SlotID <= MAX_PERK_SLOT; ++SlotID)
		{
			FString KeyName = FString::Printf(TEXT("perk_%d"), SlotID);
			FName RowName = UEnum::GetValueAsName(PlayerInfo.perks[KeyName]);
			//GEngine->AddOnScreenDebugMessage(-1, 60.f, FColor::Green,  RowName.ToString());

			FPFDataTableRowPerk* DataRow =PerksInfoDataTable->FindRow<FPFDataTableRowPerk>(RowName, ContextString);
			if (DataRow)
			{
				PerkSlots[SlotID - 1]->InitSlot(DataRow->PerkType, DataRow->PerkIcon);
				PerkSlots[SlotID - 1]->SlotID = SlotID;
			}
		}
	}

	OwnerPlayerInfo = PlayerInfo;
}

void UPFInLobbyHUD::OnCustomizePanelClosed()
{
	SelectedWeaponSlot = 0;
	SelectedSkillSlot = 0;
	SelectedPerkSlot = 0;
}

void UPFInLobbyHUD::OnWeaponSlotClicked(int32 SlotID)
{
	SelectedWeaponSlot = SlotID;
}

void UPFInLobbyHUD::OnSkillSlotClicked(int32 SlotID)
{
	SelectedSkillSlot = SlotID;
}

void UPFInLobbyHUD::OnPerkSlotClicked(int32 SlotID)
{
	SelectedPerkSlot = SlotID;
}

int32 UPFInLobbyHUD::GetSlotOfWeapon(EWeaponType WeaponType)
{
	ensureMsgf(WeaponSlots.Num() >= MAX_WEAPON_SLOT, TEXT("Not Enough Weapon on Slots"));
	for (int32 SlotID = 1; SlotID <= MAX_WEAPON_SLOT; ++SlotID)
	{
		if(WeaponSlots[SlotID - 1]->GetSlotType() == WeaponType)
			return SlotID;
	}
	return 0;
}

int32 UPFInLobbyHUD::GetSlotOfAbility(EAbilityType AbilityType)
{
	ensureMsgf(AbilitySlots.Num() >= MAX_ABILITY_SLOT, TEXT("Not Enough Ability on Slots"));
	for (int32 SlotID = 1; SlotID <= MAX_ABILITY_SLOT; ++SlotID)
	{
		if(AbilitySlots[SlotID - 1]->GetSlotType() == AbilityType)
			return SlotID;
	}
	return 0;
}

int32 UPFInLobbyHUD::GetSlotOfPerk(EPerkType PerkType)
{
	ensureMsgf(PerkSlots.Num() >= MAX_PERK_SLOT, TEXT("Not Enough Perk on Slots"));
	for (int32 SlotID = 1; SlotID <= MAX_PERK_SLOT; ++SlotID)
	{
		if (PerkSlots[SlotID - 1]->GetSlotType() == PerkType)
			return SlotID;
	}
	return 0;
}

void UPFInLobbyHUD::UpdateSelectedAbilitySlot(class UTexture2D* Icon, EAbilityType AbilityType)
{
	if (SelectedSkillSlot <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No Ability Slot Selected"));
		return;
	}

	AbilitySlots[SelectedSkillSlot - 1]->InitSlot(AbilityType, Icon);
}

EAbilityType UPFInLobbyHUD::GetSelectedAbilitySlotType()
{
	if (SelectedSkillSlot <= 0)
		return EAbilityType::AT_None;

	return AbilitySlots[SelectedSkillSlot - 1]->GetSlotType();
}

void UPFInLobbyHUD::UpdateSelectedWeaponSlot(class UTexture2D* Icon, EWeaponType WeaponType)
{
	if (SelectedWeaponSlot <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No Weapon Slot Selected"));
		return;
	}

	WeaponSlots[SelectedWeaponSlot - 1]->InitSlot(WeaponType, Icon);
}

EWeaponType UPFInLobbyHUD::GetSelectedWeaponSlotType()
{
	if (SelectedWeaponSlot <= 0)
		return EWeaponType::WT_None;

	return WeaponSlots[SelectedWeaponSlot - 1]->GetSlotType();
}

void UPFInLobbyHUD::UpdateSelectedPerkSlot(class UTexture2D* Icon, EPerkType PerkType)
{
	if (SelectedPerkSlot <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No Weapon Slot Selected"));
		return;
	}

	PerkSlots[SelectedPerkSlot - 1]->InitSlot(PerkType, Icon);
}

EPerkType UPFInLobbyHUD::GetSelectedPerkSlotType()
{
	if (SelectedPerkSlot <= 0)
		return EPerkType::PT_None;

	return PerkSlots[SelectedPerkSlot - 1]->GetSlotType();
}

void UPFInLobbyHUD::SetMapOverview() {
	UImage* mapOverviews[NUM_MAP] = {MapOverview_0, MapOverview_1};
	for (int i = 0; i < NUM_MAP; i++) {
		if (i == mapID) {
			mapOverviews[i]->SetBrushTintColor(FLinearColor(1, 1, 1, 1));
		}
		else {
			mapOverviews[i]->SetBrushTintColor(FLinearColor(1, 1, 1, 0));
		}
	}
}

void UPFInLobbyHUD::RequestNextMap() {
	mapID++;
	mapID = mapID % NUM_MAP;
	SetMapOverview();

}

void UPFInLobbyHUD::RequestPreviousMap() {
	mapID--;
	mapID = (mapID+NUM_MAP) % NUM_MAP;
	SetMapOverview();
}