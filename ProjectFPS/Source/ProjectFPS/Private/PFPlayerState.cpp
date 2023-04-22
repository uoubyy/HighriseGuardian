// Fill out your copyright notice in the Description page of Project Settings.

#include "PFPlayerState.h"
#include "Net/UnrealNetwork.h"

void APFPlayerState::OnRep_TeamID()
{
}

void APFPlayerState::OnRep_Credits(int32 OldCredits)
{
	OnCreditsChanged.Broadcast(this, Credits, Credits - OldCredits);
}

void APFPlayerState::OnRep_KilledNum(int32 OldNum)
{
	OnKilledNumChanged.Broadcast(this, KilledNum, KilledNum - OldNum);
}

void APFPlayerState::OnRep_DeadNum(int32 OldNum)
{
	OnDeadNumChanged.Broadcast(this, DeadNum, DeadNum - OldNum);
}

int32 APFPlayerState::GetCredits() const
{
	return Credits;
}

void APFPlayerState::AddCredits(int32 Delta)
{
	if (!ensure(Delta >= 0.0f))
	{
		return;
	}

	Credits += Delta;

	OnCreditsChanged.Broadcast(this, Credits, Delta);
}

bool APFPlayerState::RemoveCredits(int32 Delta)
{
	if (!ensure(Delta >= 0.0f))
	{
		return false;
	}

	if (Credits < Delta)
	{
		// Not enough credits available
		return false;
	}

	Credits -= Delta;

	OnCreditsChanged.Broadcast(this, Credits, -Delta);

	return true;
}

void APFPlayerState::IncKilledNum()
{
	KilledNum++;
}

void APFPlayerState::IncDeadNum()
{
	DeadNum++;
}

int32 APFPlayerState::GetTeamID() const
{
	return TeamID;
}

int32 APFPlayerState::GetKilledNum() const
{
	return KilledNum;
}

int32 APFPlayerState::GetDeaddNum() const
{
	return DeadNum;
}

EWeaponType APFPlayerState::GetCurrentWeaponType() const
{
	return CurrentWeaponType;
}

EWeaponType APFPlayerState::GetNextValidWeaponType() const
{
	int32 NextWeaponIndex = (CurrentWeaponIndex + 1) % (InventoryWeapons.Num());
	return InventoryWeapons[NextWeaponIndex];
}

EWeaponType APFPlayerState::GetNextValidWeaponTypeWithExclusive(EWeaponType InitWeaponType, EWeaponType ExclusiveType) const
{
	if(InitWeaponType == EWeaponType::WT_None || InitWeaponType == EWeaponType::Count || InventoryWeapons.Num() == 0)
		return EWeaponType::WT_Pistol;

	int WeaponIndex = 0;
	for (; WeaponIndex < InventoryWeapons.Num(); ++WeaponIndex)
	{
		if (InventoryWeapons[WeaponIndex] == InitWeaponType)
			break;
	}

	for (int i = 1; i < InventoryWeapons.Num(); ++i)
	{
		int32 NextWeaponIndex = (WeaponIndex + i) % (InventoryWeapons.Num());
		if(InventoryWeapons[NextWeaponIndex] != ExclusiveType)
			return InventoryWeapons[NextWeaponIndex];
	}

	return EWeaponType::WT_None;
}

void APFPlayerState::SwitchWeapon(EWeaponType WeaponType)
{
	CurrentWeaponType = WeaponType;
	for(int32 Index = 0; Index < InventoryWeapons.Num(); ++Index)
	{ 
		if (InventoryWeapons[Index] == WeaponType)
		{
			CurrentWeaponIndex = Index;
			break;
		}
	}
}

void APFPlayerState::AddWeaponToInventory(EWeaponType WeaponType)
{
	if(WeaponType == EWeaponType::WT_None || WeaponType == EWeaponType::Count) return;

	InventoryWeapons.AddUnique(WeaponType);
}

void APFPlayerState::AddAbilityToInventory(EAbilityType AbilityType)
{
	if(AbilityType == EAbilityType::AT_None || AbilityType == EAbilityType::Count) return;

	InventorySkills.AddUnique(AbilityType);
}

void APFPlayerState::AddPerkToInventory(EPerkType PerkType)
{
	if(PerkType == EPerkType::PT_None || PerkType == EPerkType::Count) return;

	InventoryPerks.AddUnique(PerkType);
}

int32 APFPlayerState::GetAbilitySlot(EAbilityType AbilityType)
{
	for (int32 ID = 0; ID < InventorySkills.Num(); ++ID)
	{
		if(InventorySkills[ID] == AbilityType)
			return ID + 1;
	}
	return 0;
}

EAbilityType APFPlayerState::GetAbilityTypeOfSlot(int32 SlotID)
{
	if(SlotID <= 0 || SlotID > InventorySkills.Num()) return EAbilityType::AT_None;
	return InventorySkills[SlotID - 1];
}

EWeaponType APFPlayerState::GetWeaponTypeOfSlot( int32 SlotID )
{
	if(SlotID <= 0 || SlotID > InventoryWeapons.Num() ) return EWeaponType::WT_None;
	return InventoryWeapons[SlotID - 1];
}

int32 APFPlayerState::GetSlotOfWeapon(EWeaponType WeaponType)
{
	for (int32 SlotID = 1; SlotID <= InventoryWeapons.Num(); ++SlotID)
	{
		if (InventoryWeapons[SlotID - 1] == WeaponType)
			return SlotID;
	}
	return 0;
}

EPerkType APFPlayerState::GetPerkTypeOfSlot(int32 SlotID)
{
	if (SlotID <= 0 || SlotID > InventoryWeapons.Num()) return EPerkType::PT_None;
	return InventoryPerks[SlotID - 1];
}

const EWeaponType APFPlayerState::GetInitialWeaponType() const
{
	ensureMsgf(InventoryWeapons.Num() > 0, TEXT("NO valid weapon inside inventory!"));
	return InventoryWeapons[0];
}

void APFPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APFPlayerState, Credits);

	DOREPLIFETIME(APFPlayerState, KilledNum);
	DOREPLIFETIME(APFPlayerState, DeadNum);

	DOREPLIFETIME_CONDITION(APFPlayerState, InventoryWeapons, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(APFPlayerState, InventorySkills, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(APFPlayerState, InventoryPerks, COND_OwnerOnly);

	DOREPLIFETIME(APFPlayerState, CurrentWeaponType);

	DOREPLIFETIME(APFPlayerState, IndexInTeam);
	DOREPLIFETIME(APFPlayerState, TeamID);
}