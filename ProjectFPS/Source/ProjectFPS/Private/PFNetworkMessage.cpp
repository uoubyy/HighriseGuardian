#include "PFNetworkMessage.h"
#include "PFEnums.h"
#include "PFGameConstant.h"

FNakamaPlayerInfo::FNakamaPlayerInfo()
{
	ResetToDefault();
}

void FNakamaPlayerInfo::ResetToDefault()
{
	status = 0;
	EWeaponType WeaponType = EWeaponType::WT_None;//GetNextWeaponType(EWeaponType::WT_None);
	for (int Slot = MIN_WEAPON_SLOT; Slot <= MAX_WEAPON_SLOT; ++Slot)
	{
		FString SlotName = FString::Printf(TEXT("weapon_%d"), Slot);
		EWeaponType& TargetWeapon = weapons.FindOrAdd(SlotName);
		TargetWeapon = WeaponType;
	}

	EAbilityType AbilityType = EAbilityType::AT_None;//GetNextAbilityType(EAbilityType::AT_None);
	for (int Slot = MIN_ABILITY_SLOT; Slot <= MAX_ABILITY_SLOT; ++Slot)
	{
		FString SlotName = FString::Printf(TEXT("skill_%d"), Slot);
		EAbilityType& TargetAbility = skills.FindOrAdd(SlotName);
		TargetAbility = AbilityType;
	}

	EPerkType PerkType = EPerkType::PT_None;//GetNextAbilityType(EAbilityType::AT_None);
	for (int Slot = MIN_PERK_SLOT; Slot <= MAX_PERK_SLOT; ++Slot)
	{
		FString SlotName = FString::Printf(TEXT("perk_%d"), Slot);
		EPerkType& TargetPerk = perks.FindOrAdd(SlotName);
		TargetPerk = PerkType;
	}
}
