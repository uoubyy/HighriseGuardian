// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PFEnums.h"
#include "PFWeaponBase.generated.h"

UENUM(BlueprintType)
enum class EWeaponClassType : uint8 { WCT_Base, WCT_BaseClient, WCT_BaseServer };

UCLASS()
class PROJECTFPS_API APFWeaponBase : public AActor {
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APFWeaponBase();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	static APFWeaponBase* SpawnWeapon(UWorld* World, UClass* Class, FTransform const& Transform, AActor* WeaponOwner);

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class USkeletalMeshComponent* WeaponMesh;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EWeaponClassType WeaponClassType = EWeaponClassType::WCT_Base;

	UPROPERTY(EditAnywhere)
	EWeaponType WeaponType;

	UPROPERTY(EditAnywhere)
	EWeaponStatusType WeaponStatus = EWeaponStatusType::WST_NotEquipped;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName AttachedSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Replicated, Category = WeaponServerEditOnly, meta = (EditCondition = "WeaponClassType == EWeaponClassType::WCT_BaseServer"))
	int MaxAmmo;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = WeaponServerEditOnly, meta = (EditCondition = "WeaponClassType == EWeaponClassType::WCT_BaseServer"))
	int Ammo;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Replicated, Category = WeaponServerEditOnly, meta = (EditCondition = "WeaponClassType == EWeaponClassType::WCT_BaseServer"))
	bool IsAuto;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = WeaponServerEditOnly, meta = (EditCondition = "WeaponClassType == EWeaponClassType::WCT_BaseServer"))
	bool IsLineTraceWeapon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Replicated, Category = WeaponServerEditOnly, meta = (EditCondition = "WeaponClassType == EWeaponClassType::WCT_BaseServer"))
	float WeaponDamage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Replicated, Category = WeaponServerEditOnly, meta = (EditCondition = "WeaponClassType == EWeaponClassType::WCT_BaseServer"))
	float FireRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = WeaponServerEditOnly)
	float LastFireTime;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Replicated, Category = WeaponServerEditOnly, meta = (EditCondition = "WeaponClassType == EWeaponClassType::WCT_BaseServer"))
	int BulletsPerFire;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Replicated, Category = WeaponServerEditOnly, meta = (EditCondition = "WeaponClassType == EWeaponClassType::WCT_BaseServer"))
	float ScatteringAngle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UNiagaraComponent* MuzzleFlash;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WeaponClientEditOnly)
	USoundBase* FireSound;

	virtual void BeginPlay() override;

public:

	UFUNCTION(BlueprintCallable)
	EWeaponType GetWeaponType() const { return WeaponType; }

	UFUNCTION()
	virtual void OnWeaponEquiped(UPrimitiveComponent* TargetPlayer, FName SocketName);

	UFUNCTION()
	virtual void OnWeaponDropped();

	UFUNCTION(BlueprintImplementableEvent)
	void OnFireBP();

	UFUNCTION(BlueprintCallable)
	virtual void OnReLoad();

	UFUNCTION(BlueprintCallable)
	virtual bool OnAddAmmo(int AmmoAmount);

	UFUNCTION(BlueprintCallable)
	int GetMaxAmmo() const { return MaxAmmo; }

	UFUNCTION(BlueprintCallable)
	int GetCurrAmmo() const { return Ammo; }

	UFUNCTION(BlueprintCallable)
	virtual bool CanFire();

	UFUNCTION(BlueprintCallable)
	USkeletalMeshComponent* GetMesh() const { return WeaponMesh; }

	UFUNCTION(BlueprintCallable)
	bool IsAutoWeapon() const { return IsAuto; };

	UFUNCTION(BlueprintCallable)
	bool IsLineTraceShoot() const { return IsLineTraceWeapon; }

	UFUNCTION(BlueprintCallable)
	float GetWeaponDamage() const { return WeaponDamage; }

	UFUNCTION(BlueprintCallable)
	int GetBulletsPerFire() const { return BulletsPerFire; }

	UFUNCTION(BlueprintCallable)
	float GetScatteringAngle() const { return ScatteringAngle; }

	UFUNCTION(BlueprintCallable)
	virtual void OnPreEquip();

	UFUNCTION(BlueprintCallable)
	virtual void OnPreDrop();

	UFUNCTION(BlueprintCallable)
	FName GetAttachedSocketName() const { return AttachedSocketName; }
};
