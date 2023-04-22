// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PFEnums.h"
#include "PFNetworkMessage.h"
#include "PFInGameHUD.generated.h"

class APFPlayerState;

/**
 *
 */
UCLASS()
class PROJECTFPS_API UPFInGameHUD : public UUserWidget
{
	GENERATED_BODY()
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EWeaponType CurrentWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	int AmmoAmount1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	int AmmoAmount2;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	int AmmoAmount3;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	int AmmoAmount4;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	int MaxAmmoAmount1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	int MaxAmmoAmount2;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	int MaxAmmoAmount3;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	int MaxAmmoAmount4;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	int KilledNum;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	int DeadNum;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	int Credits;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	int HealthPercent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FString UserName;

	// With BindWidget, UserWidget has the same name and type in BP 
	// will bind to this variable automatically
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* Ammo1;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* MaxAmmo1;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* Ammo2;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* MaxAmmo2;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* Ammo3;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* MaxAmmo3;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* Ammo4;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* MaxAmmo4;

	UPROPERTY( BlueprintReadWrite, meta = ( BindWidget ) )
	class UCanvasPanel* RootPanel;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UCanvasPanel* SkillSlot1;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UImage* SkillIcon1;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UCanvasPanel* SkillSlot2;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UImage* SkillIcon2;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UImage* WeaponIcon1;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UImage* WeaponIcon2;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UImage* WeaponIcon3;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UImage* WeaponIcon4;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UImage* WeaponBg_1;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UImage* WeaponBg_2;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UImage* WeaponBg_3;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UImage* WeaponBg_4;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UButton* DebugButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UListView* KillRecordListView;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UProgressBar* HealthBar;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UPFKillRecordWidget> KillRecordWidgetClass;

	UPROPERTY(EditDefaultsOnly)
	int32 MaxVisibleKillRecords;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UDataTable* SkillsInfoDataTable;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UDataTable* WeaponInfoDataTable;

	UFUNCTION(BlueprintImplementableEvent)
	void ChangeAbilitySlotIcon(int32 SlotID, class UTexture2D* AbilityIcon);

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UPFHitIndicatorWidget> HitIndicatorWidgetClass;

	UPROPERTY(BlueprintReadOnly)
	TMap<FString, class UPFHitIndicatorWidget*> HitIndicators;

	UPROPERTY(BlueprintReadOnly)
	TArray<TSoftObjectPtr<AActor>> PickupActors;

	UPROPERTY(BlueprintReadOnly)
	TArray<class UPFPickupIndicatorWidget*> PickupIndicators;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UPFPickupIndicatorWidget> PickupIndicatorWidgetClass;

	UPROPERTY(BlueprintReadWrite, meta = ( BindWidget ))
	class UCanvasPanel* HitIndicatorCanvas;

	TArray<EWeaponType> WeaponList;
	TArray<EAbilityType> AbilityList;
public:

	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable)
	void InitInGameHUD(class UPFAttributeComponent* AttributeComp);

	UFUNCTION(BlueprintCallable)
	void RefreshInGameHUD(APFPlayerState* PlayerState);

	UFUNCTION(BlueprintCallable)
	void SetWeaponSlot(EWeaponType Weapon);

	UFUNCTION(BlueprintCallable)
	int GetAbilitySlot(EAbilityType ability);

	UFUNCTION()
	void OnDeadNumChanged(APFPlayerState* PlayerState, int32 NewNum, int32 Delta);

	UFUNCTION()
	void OnKilledNumChanged(APFPlayerState* PlayerState, int32 NewNum, int32 Delta);

	UFUNCTION()
	void OnCreditsChanged(APFPlayerState* PlayerState, int32 NewCredits, int32 Delta);

	UFUNCTION()
	void OnAmmoAmountChanged(int AmmoAmount);

	UFUNCTION()
	void OnMaxAmmoChanged(int MaxAmmoAmount);

	UFUNCTION(BlueprintCallable)
	void OnHealthChanged(AActor* InstigatorActor, class UPFAttributeComponent* OwningComp, float NewValue, float Delta);

	UFUNCTION(BlueprintCallable)
	void OnCauseDamage(AActor* VictimActor, float NewHealth, float Delta);

	UFUNCTION(BlueprintImplementableEvent, Category = GamePlay)
	void OnAmmoAmountChangedBP(int AmmoAmount, int MaxAmmoAmount);

	UFUNCTION(BlueprintImplementableEvent, Category = GamePlay)
	void ShowHitmarker();

	UFUNCTION(BlueprintImplementableEvent, Category = GamePlay)
	void ShowKillIcon();

	UFUNCTION(BlueprintImplementableEvent, Category = GamePlay)
	void OnMatchStart(int MatchRoundDuration);

	UFUNCTION(BlueprintImplementableEvent, Category = GamePlay)
	void OnMatchStateChanged(FName StateName);

	UFUNCTION(BlueprintImplementableEvent, Category = GamePlay)
	void OnMatchScoreChanged(int32 LeftTeam, int32 RightTeam);

	UFUNCTION(BlueprintNativeEvent, Category = GamePlay)
	void OnWeaponChanged(EWeaponType NewWeapon, EWeaponType OldWeapon);

	UFUNCTION(BlueprintImplementableEvent)
	void OnCountDownChanged(int CountDown);

	UFUNCTION(BlueprintImplementableEvent)
	void OnAbilityTrigger(AActor* InstigatorActor, EAbilityType AbilityType, float Anticipation, float Duration, float CoolDown);

	UFUNCTION(BlueprintImplementableEvent)
	void OnAbilityStart(AActor* InstigatorActor, EAbilityType AbilityType, float Duration, float CoolDown);

	UFUNCTION(BlueprintImplementableEvent)
	void OnAbilityEnd(AActor* InstigatorActor, EAbilityType AbilityType, float CoolDown);

	UFUNCTION(BlueprintImplementableEvent)
	void ToggleSettingsPanel();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void OnKillRecordReported(const FKillRecord KillRecord);

	UFUNCTION(BlueprintCallable)
	void TogglePickupIndicators(EPowerupType PickupType, bool Value);

	UFUNCTION(BlueprintImplementableEvent, Category = GamePlay)
	void SpawnHurtNum(float WeaponDamage);
private:
	UFUNCTION(BlueprintCallable)
	void OpenDebugLevel();
};