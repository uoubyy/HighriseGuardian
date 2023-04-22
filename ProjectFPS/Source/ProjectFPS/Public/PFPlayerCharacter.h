// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PFEnums.h"
#include "PFWeaponBaseServer.h"
#include "PFWeaponBaseClient.h"
#include "PFPerkBaseComponent.h"
#include "PFAbilityBaseComponent.h"

#include "PFPlayerCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCharacterOnFireDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCharacterInitCompleteDelegate);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCharacterRespawnDelegate);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSwitchWeaponFinishDelegate, APFWeaponBase*, OldWeapon, APFWeaponBase*, NewWeapon);

DECLARE_DELEGATE_OneParam(FSwitchWeaponDelegate, const int32);
DECLARE_DELEGATE_OneParam(FShowScoreBoardDelegate, const bool ShowOrHide);

UCLASS()
class PROJECTFPS_API APFPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APFPlayerCharacter();

	virtual void OnRep_PlayerState() override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components)
	class UCameraComponent* PlayerCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components)
	class USkeletalMeshComponent* FPMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components)
	class UPFAttributeComponent* AttributeComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components)
	class UStaticMeshComponent* PlayerOutlineMesh;

	UPROPERTY(EditDefaultsOnly, Category = Weapon)
	FName WeaponServerSocket;

	UPROPERTY(EditDefaultsOnly, Category = Weapon)
	FName WeaponClientSocket;

protected:
	virtual void Restart() override;

	virtual void PawnClientRestart() override;

	virtual void PossessedBy(AController* NewController) override;

	virtual void PostInitializeComponents() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	TMap<EWeaponType, TSubclassOf<APFWeaponBaseServer>> ServerWeaponClasses;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	TMap<EWeaponType, TSubclassOf<APFWeaponBaseClient>> ClientWeaponClasses;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	//UAnimMontage* FireAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	UAnimMontage* DeathAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Perk)
	TMap<EPerkType, TSubclassOf<UPFPerkBaseComponent>> PerkComponentClasses;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ability)
	TMap<EAbilityType, TSubclassOf<UPFAbilityBaseComponent>> AbilityComponentClasses;

	UPROPERTY(EditAnywhere, Category = UI)
	TSubclassOf<UUserWidget> InGameHUDClass;

	UPROPERTY(EditAnywhere, Category = UI)
	TSubclassOf<UUserWidget> ScoreboardClass;

	UPROPERTY(EditDefaultsOnly, Category = Team)
	FColor TeammatesColor;

	UPROPERTY(EditDefaultsOnly, Category = Team)
	FColor EnemyColor;

	UPROPERTY()
	class UPFScoreboardWidget* Scoreboard;

	UPROPERTY()
	bool bInputReverse;

	UPROPERTY()
	float MouseSensitivity = 1.0f;

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
	void RequestSpawnEntranceWeapon(/*EWeaponType WeaponType*/);

	UFUNCTION()
	void OnHealthChanged(AActor* InstigatorActor, class UPFAttributeComponent* OwningComp, float NewHealth, float Delta);

	UFUNCTION()
	void OnCauseDamage(AActor* VictimActor, float NewHealth, float Delta);

	UFUNCTION()
	void OnMatchStateChange(FName NewState, FName OldState);

	UFUNCTION()
	void OnMatchScoreChange(int32 TeamAScore, int32 TeamBScore);

	UFUNCTION()
	void OnMatchStageCountDownChange(int CountDown);

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = UI)
	class UPFInGameHUD* InGameHUD;

	UPROPERTY(BlueprintAssignable, Category = "GamePlay")
	FCharacterOnFireDelegate OnIsFiringDelegate;

	UPROPERTY(BlueprintAssignable, Category = "GamePlay")
	FOnCharacterInitCompleteDelegate OnCharacterInitCompleteDelegate;

	UPROPERTY(BlueprintAssignable, Category = "GamePlay")
	FOnCharacterRespawnDelegate OnCharacterRespawnDelegate;

	UPROPERTY(BlueprintAssignable, Category = "GamePlay")
	FOnSwitchWeaponFinishDelegate OnSwitchWeaponClientFinishDelegate;

	UPROPERTY(BlueprintAssignable, Category = "GamePlay")
	FOnSwitchWeaponFinishDelegate OnSwitchWeaponServerFinishDelegate;

	UFUNCTION(BlueprintCallable)
	void ReverseInput(bool val) { bInputReverse = val; }

	UFUNCTION(BlueprintCallable)
	bool IsInputReverse() { return bInputReverse; }

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void OnDeath();

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void OnRespawn();

	UFUNCTION(BlueprintCallable)
	void EnterDamageImmune();

	UFUNCTION(BlueprintCallable)
	void RequestSwitchWeaponClient(EWeaponType WeaponType = EWeaponType::WT_None);

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
	void RequestSwitchWeaponServer(EWeaponType WeaponType);

	UFUNCTION(BlueprintCallable)
	void RequestSwitchWeaponWithID(int32 SlotID);

	UFUNCTION(BlueprintImplementableEvent, Category = "GamePlay")
	void SetSwitchWeaponAnim();

	UFUNCTION(BlueprintCallable)
	float GetGravityZ() const;

	UFUNCTION(BlueprintCallable)
	FORCEINLINE APFWeaponBaseServer* GetCurrentWeaponServer() { return CurrentWeaponServer; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE APFWeaponBaseClient* GetCurrentWeaponClient() { return CurrentWeaponClient; }

	UFUNCTION(BlueprintCallable)
	APFWeaponBaseServer* GetWeaponServerOfType(EWeaponType WeaponType);

	UFUNCTION(BlueprintCallable)
	APFWeaponBaseClient* GetWeaponClientOfType(EWeaponType WeaponType);

	UFUNCTION(BlueprintCallable)
	TArray<UMeshComponent*> GetAllThirdPersonMeshes();

	UFUNCTION(BlueprintImplementableEvent)
	TArray<UMeshComponent*> GetThirdPersonMeshesFromBP();

	UFUNCTION(BlueprintCallable)
	TArray<UMeshComponent*> GetAllFirstPersonMeshes();

	UFUNCTION(BlueprintImplementableEvent)
	TArray<UMeshComponent*> GetFirstPersonMeshesFromBP();

	UFUNCTION(BlueprintCallable)
	void UpdateAmmoUI();

	UFUNCTION(BlueprintImplementableEvent, Category = GamePlay)
	void UpdateHealthBar(float NewValue, float Delta);

	UFUNCTION(BlueprintImplementableEvent)
	void InitCharacterBlueprintComponent();

	UFUNCTION()
	void OnTeamChanged(int Team);

	UFUNCTION(BlueprintCallable)
	int32 GetTeamID() const { return TeamID; }

	UFUNCTION(BlueprintCallable)
	bool GetIsLocalPlayer() const;// { return IsLocalPlayer; }

	UFUNCTION(BlueprintImplementableEvent)
	void ChangePlayerOutlineMeshColor(bool IsTeammate);

	virtual void FellOutOfWorld(const class UDamageType& dmgType) override;

	UFUNCTION(BlueprintCallable)
	void RequestChangeMousesenSitivity(float Value);

	// TODO Yanyi Bao, 11/30/2022
	UFUNCTION(Client, Unreliable)
	void ClientAddAmmo(int32 AmmoBenefits);

	UFUNCTION(BlueprintCallable)
	class UPFAttributeComponent* GetAttributeComponent() { return AttributeComp; }

	UFUNCTION(BlueprintCallable)
	class UPFAbilityBaseComponent* GetAbilityComponent(EAbilityType AbilityType) const;

	UFUNCTION(BlueprintCallable)
	class UPFAbilityBaseComponent* GetAbilityComponentBySlotID(int32 SlotID) const;

	UFUNCTION(BlueprintCallable)
	class USkeletalMeshComponent* GetFirstPersonSkeletonMesh() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	class UNiagaraComponent* GetDamageBuffNiagara();

	UFUNCTION(BlueprintCallable)
	float GetTurnAxisInput() const { return TurnAxisInput; }

protected:

	/** Fires a projectile. */
	void OnFire();

	void StartFire();

	void StopFire();

	void OnReLoad();

	void NextWeapon();

	void LastWeapon();

	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles stafing movement, left and right */
	void MoveRight(float Val);

	void ViewLookUp(float val);

	void ViewLookRight(float val);

	UPROPERTY(ReplicatedUsing = "OnRep_WeaponServer")
	class APFWeaponBaseServer* CurrentWeaponServer;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	int32 TeamID;

	UPROPERTY()
	class APFWeaponBaseClient* CurrentWeaponClient;

	UPROPERTY(VisibleAnywhere)
	TMap<EWeaponType, class APFWeaponBaseServer*> WeaponListServer;

	UPROPERTY(VisibleAnywhere)
	TMap<EWeaponType, class APFWeaponBaseClient*> WeaponListClient;

	UPROPERTY(VisibleAnywhere, Replicated)
	TArray<class UPFAbilityBaseComponent*> AbilityComponents; // Key is ability slot id

	UFUNCTION()
	void OnRep_WeaponServer(class APFWeaponBaseServer* OldValue);

	UFUNCTION()
	void SwitchWeaponClient(EWeaponType WeaponType);

	UFUNCTION()
	void DropWeaponClient();

	UFUNCTION()
	void EquipWeaponClient(EWeaponType NewWeaponType);

	void ClientFire(FVector& BulletTargetPoint,TArray<APFPlayerCharacter*>& TargetPlayers);

	UFUNCTION(Server, Reliable)
	void ServerFire(const FVector& BulletTargetPoint, const TArray<APFPlayerCharacter*>& TargetPlayers);

	UFUNCTION(Server, Reliable)
	void ReLoadServer();

	void ShowScoreBoard();

private:
	bool IsLocalPlayer = false;

	int32 LocalPlayerTeamID;

	void OnCharacterInitComplete();

	UFUNCTION(Server, Reliable, WithValidation)
	void ChangeMousesenSitivity(float Value);

	void ToggleSettingsPanel();

	bool IsInitComplete;

	bool IsFiring;

	FTimerHandle FireHandle;

	void RefreshInGameHUD(class APFPlayerState* PFPlayerState);

	float TurnAxisInput = 0.0f;
};
