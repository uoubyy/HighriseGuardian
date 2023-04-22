// Copyright FPS Team. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PFWeaponBase.h"
#include "PFWeaponBaseClient.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTFPS_API APFWeaponBaseClient : public APFWeaponBase
{
	GENERATED_BODY()

public:

	APFWeaponBaseClient();

protected:

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = WeaponClientEditOnly)
	TSubclassOf<UCameraShakeBase> CameraShakeClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = WeaponClientEditOnly)
	class UNiagaraSystem* NSHitHardSurface;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = WeaponClientEditOnly)
	class UNiagaraSystem* NSHitPlayer;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = WeaponClientEditOnly)
	class UNiagaraSystem* NSBulletHole;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = WeaponClientEditOnly)
	bool ScopeCameraEnabled;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = WeaponClientEditOnly)
	FName ScopeCameraSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = WeaponClientEditOnly)
	TSubclassOf<UAnimInstance> WeaponEquippedAnimClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	class UAnimSequence* FireAnimation;

public:

	virtual void OnWeaponEquiped(UPrimitiveComponent* TargetPlayer, FName SocketName) override;
	
	virtual void OnWeaponDropped() override;

	virtual bool CanFire() override;

	UFUNCTION(BlueprintCallable)
	void SyncWeaponProperties(class APFWeaponBaseServer* WeaponServer);

	UFUNCTION(BlueprintCallable)
	bool GetScopeCameraEnabled() const { return ScopeCameraEnabled; }

	UFUNCTION(BlueprintCallable)
	bool EquipScopeCamera(class ACameraActor* ScopeCamera);

	UFUNCTION()
	virtual void OnFireClient(const FVector CameraLocation, const FRotator SpawnRotation, class APFPlayerController* PlayerController, FVector& BulletTarget, TArray<APFPlayerCharacter*>& TargetPlayers);
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	UNiagaraComponent* GetBuffNiagara();

	UFUNCTION(BlueprintNativeEvent)
	void SetEffectsOnEquiped();

	UFUNCTION(BlueprintNativeEvent)
	void SetEffectsOnDropped();

	UFUNCTION()
	class UAnimSequence* GetFireAnimation() const;
};
