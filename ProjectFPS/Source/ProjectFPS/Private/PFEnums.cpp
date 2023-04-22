#include "PFEnums.h"

EWeaponType GetNextWeaponType(EWeaponType Current)
{
	if(Current == EWeaponType::Count || Current == EWeaponType::WT_None) return EWeaponType::WT_SemiAuto;

	EWeaponType NextWeaponType = EWeaponType((int)Current + 1);
	if(NextWeaponType == EWeaponType::Count || NextWeaponType == EWeaponType::WT_None) return EWeaponType::WT_SemiAuto;
	return NextWeaponType;
}

EWeaponType GetWeaponTypeByID(int32 ID)
{
	if(ID <= 0 || ID >= (int)EWeaponType::Count) return EWeaponType::WT_None;

	return EWeaponType(ID);
}

EAbilityType GetNextAbilityType(EAbilityType Current)
{
	if (Current == EAbilityType::Count || Current == EAbilityType::AT_None) return EAbilityType::AT_Dash;

	EAbilityType NextAbilityType = EAbilityType((int)Current + 1);
	if (NextAbilityType == EAbilityType::Count || NextAbilityType == EAbilityType::AT_None) return EAbilityType::AT_Dash;
	return NextAbilityType;
}

EPerkType GetNextPerkType(EPerkType Current)
{
	if (Current == EPerkType::Count || Current == EPerkType::PT_None) return EPerkType::PT_FOF;

	EPerkType NextPerkType = EPerkType((int)Current + 1);
	if (NextPerkType == EPerkType::Count || NextPerkType == EPerkType::PT_None) return EPerkType::PT_FOF;
	return NextPerkType;
}

FString GetModifierTypeName(EPropertyModifierType ModifierType)
{
	switch (ModifierType)
	{
	case EPropertyModifierType::PMT_Damage:
		return "PMT_Damage";
	case EPropertyModifierType::PMT_Health:
		return "PMT_Health";
	case EPropertyModifierType::PMT_Movement:
		return "PMT_Movement";
	default:
		return "None";
	}
	return "None";
}

FString ConvertPropertyModifierTypeEnumToString(EPropertyModifierType TargetProperty)
{
	switch (TargetProperty)
	{
	case EPropertyModifierType::PMT_None:
		return "EPropertyModifierType::PMT_None";
		break;
	case EPropertyModifierType::PMT_Movement:
		return "EPropertyModifierType::PMT_Movement";
		break;
	case EPropertyModifierType::PMT_Damage:
		return "EPropertyModifierType::PMT_Damage";
		break;
	case EPropertyModifierType::PMT_Health:
		return "EPropertyModifierType::PMT_Health";
		break;
	case EPropertyModifierType::PMT_All:
		return "EPropertyModifierType::PMT_All";
		break;
	case EPropertyModifierType::Count:
		break;
	default:
		break;
	}
	return "EPropertyModifierType::None";
}

