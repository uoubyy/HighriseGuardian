// Copyright FPS Team. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PFNetworkMessage.h"
#include "PFInLobbyHUD.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTFPS_API UPFInLobbyHUD : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable)
	void InitLobbyHUD(const FNakamaPlayerInfo& PlayerInfo, const FString& NakamaMatchID);

	UFUNCTION(BlueprintCallable)
	void OnCustomizePanelClosed();

	UFUNCTION(BlueprintCallable)
	void OnWeaponSlotClicked(int32 SlotID);

	UFUNCTION(BlueprintCallable)
	void OnSkillSlotClicked(int32 SlotID);

	UFUNCTION(BlueprintCallable)
	void OnPerkSlotClicked(int32 SlotID);

	UFUNCTION(BlueprintCallable)
	int32 GetSlotOfWeapon(EWeaponType WeaponType);

	UFUNCTION(BlueprintCallable)
	int32 GetSlotOfAbility(EAbilityType AbilityType);

	UFUNCTION(BlueprintCallable)
	int32 GetSlotOfPerk(EPerkType PerkType);

	UFUNCTION(BlueprintCallable)
	void UpdateSelectedAbilitySlot(class UTexture2D* Icon, EAbilityType AbilityType);

	UFUNCTION(BlueprintCallable)
	EAbilityType GetSelectedAbilitySlotType();

	UFUNCTION(BlueprintCallable)
	void UpdateSelectedWeaponSlot(class UTexture2D* Icon, EWeaponType WeaponType);

	UFUNCTION(BlueprintCallable)
	EWeaponType GetSelectedWeaponSlotType();

	UFUNCTION(BlueprintCallable)
	void UpdateSelectedPerkSlot(class UTexture2D* Icon, EPerkType PerkType);

	UFUNCTION(BlueprintCallable)
	EPerkType GetSelectedPerkSlotType();

	UFUNCTION(BlueprintCallable)
	void RequestNextMap();

	UFUNCTION(BlueprintCallable)
	void RequestPreviousMap();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UDataTable* WeaponsInfoDataTable;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UDataTable* SkillsInfoDataTable;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UDataTable* PerksInfoDataTable;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UImage* Background;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UImage* MapOverview_0;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UImage* MapOverview_1;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* MatchID;

	UPROPERTY(BlueprintReadWrite)
	TArray<class UPFAbilitySlotWidget*> AbilitySlots;

	UPROPERTY(BlueprintReadWrite)
	TArray<class UPFWeaponSlotWidget*> WeaponSlots;

	UPROPERTY(BlueprintReadWrite)
	TArray<class UPFPerkSlotWidget*> PerkSlots;

	// start from 1
	UPROPERTY(BlueprintReadWrite)
	int32 SelectedWeaponSlot;

	// start from 1
	UPROPERTY(BlueprintReadWrite)
	int32 SelectedSkillSlot;

	// start from 1
	UPROPERTY(BlueprintReadWrite)
	int32 SelectedPerkSlot;

	// start from 
	UPROPERTY(BlueprintReadWrite)
	int32 mapID;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void UpdateMatchPresences(const TArray<FNakamaMsgJoin>& MatchPresences);

private:
	UPROPERTY()
	FNakamaPlayerInfo OwnerPlayerInfo;

	UFUNCTION()
	void SetMapOverview();
};
