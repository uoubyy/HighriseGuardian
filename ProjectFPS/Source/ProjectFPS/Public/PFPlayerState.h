// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "PFEnums.h"
#include "PFPlayerState.generated.h"

class APFlayerState;

// Event Handler for Credits
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnCreditsChanged, APFPlayerState*, PlayerState, int32, NewCredits, int32, Delta);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnKilledNumChanged, APFPlayerState*, PlayerState, int32, NewNum, int32, Delta);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnDeadNumChanged, APFPlayerState*, PlayerState, int32, NewNum, int32, Delta);

/**
 *
 */
UCLASS()
class PROJECTFPS_API APFPlayerState : public APlayerState
{
	GENERATED_BODY()

protected:

	UPROPERTY(VisibleAnywhere, ReplicatedUsing = "OnRep_TeamID", Category = "GamePlay")
	int32 TeamID;

	UPROPERTY(VisibleAnywhere, Replicated, Category = "GamePlay")
	int32 IndexInTeam;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing = "OnRep_KilledNum", Category = "GamePlay")
	int32 KilledNum;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing = "OnRep_DeadNum", Category = "GamePlay")
	int32 DeadNum;

	UPROPERTY(EditDefaultsOnly, ReplicatedUsing = "OnRep_Credits", Category = "Credits")
	int32 Credits;

	UPROPERTY(VisibleAnywhere, Replicated, Category = "Weapon")
	TArray<EWeaponType> InventoryWeapons;

	UPROPERTY(VisibleAnywhere, Replicated, Category = "Weapon")
	EWeaponType CurrentWeaponType;

	UPROPERTY(VisibleAnywhere, Replicated, Category = "Ability")
	TArray<EAbilityType> InventorySkills;

	UPROPERTY(VisibleAnywhere, Replicated, Category = "Perk")
	TArray<EPerkType> InventoryPerks;

	UPROPERTY()
	int32 CurrentWeaponIndex;

	UFUNCTION()
	void OnRep_TeamID();

	UFUNCTION()
	void OnRep_Credits(int32 OldCredits);

	UFUNCTION()
	void OnRep_KilledNum(int32 OldNum);

	UFUNCTION()
	void OnRep_DeadNum(int32 OldNum);

public:

	UFUNCTION(BlueprintCallable, Category = "Credits")
	int32 GetCredits() const;

	UFUNCTION(BlueprintCallable, Category = "Credits")
	void AddCredits(int32 Delta);

	UFUNCTION(BlueprintCallable, Category = "Credits")
	bool RemoveCredits(int32 Delta);

	UFUNCTION(BlueprintCallable, Category = "Credits")
	void IncKilledNum();

	UFUNCTION(BlueprintCallable, Category = "Credits")
	void IncDeadNum();

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnCreditsChanged OnCreditsChanged;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnKilledNumChanged OnKilledNumChanged;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnDeadNumChanged OnDeadNumChanged;

	UFUNCTION()
	void SetTeamID(int NewTeam) { TeamID = NewTeam; }

	UFUNCTION(BlueprintCallable)
	int32 GetTeamID() const;

	UFUNCTION()
	void SetIndexInTeam(int Index) { IndexInTeam = Index; }

	UFUNCTION()
	int32 GetIndexInTeam() const { return IndexInTeam; }

	UFUNCTION()
	int32 GetKilledNum() const;

	UFUNCTION()
	int32 GetDeaddNum() const;

	UFUNCTION(BlueprintCallable)
	TArray<EWeaponType> GetInventoryWeapons() const { return InventoryWeapons; }

	UFUNCTION(BlueprintCallable)
	TArray<EAbilityType> GetInventoryAbilities() const { return InventorySkills; }

	UFUNCTION(BlueprintCallable)
	TArray<EPerkType> GetInventoryPerks() const { return InventoryPerks; }

	// TODO
	// Get weapon by current weapon index
	UFUNCTION(BlueprintCallable)
	EWeaponType GetCurrentWeaponType() const;

	UFUNCTION(BlueprintCallable)
	EWeaponType GetNextValidWeaponType() const;

	UFUNCTION(BlueprintCallable)
	EWeaponType GetNextValidWeaponTypeWithExclusive(EWeaponType InitWeaponType, EWeaponType ExclusiveType) const;

	UFUNCTION(BlueprintCallable)
	void SwitchWeapon(EWeaponType WeaponType);

	UFUNCTION(BlueprintCallable)
	void AddWeaponToInventory(EWeaponType WeaponType);

	UFUNCTION(BlueprintCallable)
	void AddAbilityToInventory(EAbilityType AbilityType);

	UFUNCTION(BlueprintCallable)
	void AddPerkToInventory(EPerkType PerkType);

	UFUNCTION(BlueprintCallable)
	const EWeaponType GetInitialWeaponType() const;

	// TODO
	UFUNCTION(BlueprintCallable)
	int32 GetAbilitySlot(EAbilityType AbilityType);

	UFUNCTION(BlueprintCallable)
	EAbilityType GetAbilityTypeOfSlot(int32 SlotID);

	UFUNCTION(BlueprintCallable)
	EWeaponType GetWeaponTypeOfSlot(int32 SlotID);

	UFUNCTION(BlueprintCallable)
	int32 GetSlotOfWeapon(EWeaponType EWeaponType);

	UFUNCTION(BlueprintCallable)
	EPerkType GetPerkTypeOfSlot(int32 SlotID);
};
