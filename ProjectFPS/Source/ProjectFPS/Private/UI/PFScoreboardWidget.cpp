// Copyright FPS Team. All Rights Reserved.


#include "UI/PFScoreboardWidget.h"
#include "PFPlayerState.h"
#include "DataTable/PFDataTableRowSkill.h"
#include "DataTable/PFDataTableRowWeapon.h"
#include "PFGameConstant.h"
void UPFScoreboardWidget::ToggleScoreboard(bool Visible)
{
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (!PlayerController) return;

	if (Visible)
	{
		this->AddToViewport();

		PlayerController->SetInputMode(FInputModeGameAndUI());
		PlayerController->SetShowMouseCursor(true);
	}
	else
	{
		this->RemoveFromParent();
		PlayerController->SetInputMode(FInputModeGameOnly());
		PlayerController->SetShowMouseCursor(false);
	}
}


TArray<UTexture2D*> UPFScoreboardWidget::GetWeaponIcon(APFPlayerState* inPlayerState) {

	TArray<UTexture2D*> weapons;
	//Init Weapon Panel
	static const FString ContextString;
	if (WeaponInfoDataTable && inPlayerState) {
		for (int i = 1; i <= MAX_WEAPON_SLOT; i++) {
			EWeaponType weaponType = inPlayerState->GetWeaponTypeOfSlot(i);
			FName weaponName = UEnum::GetValueAsName(inPlayerState->GetWeaponTypeOfSlot(i));
			FPFDataTableRowWeapon* DataRow = WeaponInfoDataTable->FindRow<FPFDataTableRowWeapon>(weaponName, ContextString);
			if (DataRow)
			{
				UTexture2D* Icon = DataRow->KillRecordWeaponIcon;
				weapons.Add(Icon);
			}
		}
	}
	return weapons;
}


TArray<UTexture2D*> UPFScoreboardWidget::GetSkillIcon(APFPlayerState* inPlayerState) {

	TArray<UTexture2D*> skills;
	static const FString ContextString;
	//Init Skill Panel
	if (SkillsInfoDataTable && inPlayerState)
	{
		for (int32 SlotID = 1; SlotID <= MAX_ABILITY_SLOT; ++SlotID)
		{
			FString KeyName = FString::Printf(TEXT("skill_%d"), SlotID);
			FName RowName = UEnum::GetValueAsName(inPlayerState->GetAbilityTypeOfSlot(SlotID));
			FPFDataTableRowSkill* DataRow = SkillsInfoDataTable->FindRow<FPFDataTableRowSkill>(RowName, ContextString);
			if (DataRow)
			{
				UTexture2D* Icon = DataRow->AbilityIcon;
				skills.Add(Icon);
			}
		}
	}
	return skills;
}