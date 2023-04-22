// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PFWeaponBase.h"
#include "PFEnums.h"
#include "PFProjectileBase.h"
#include "PFWeaponBaseServer.generated.h"

UCLASS()
class PROJECTFPS_API APFWeaponBaseServer : public APFWeaponBase
{
	GENERATED_BODY()

public:

	APFWeaponBaseServer();

protected:
	virtual void PostInitializeComponents() override;

	UPROPERTY(EditAnywhere)
	class USphereComponent* SphereCollision;

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USceneComponent* MuzzlePoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	TSubclassOf<APFProjectileBase> BulletBaseClass;

	UFUNCTION()
	void OnWeaponBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:

	virtual void OnWeaponEquiped(UPrimitiveComponent* TargetPlayer, FName SocketName) override;

	virtual void OnWeaponDropped() override;

	UFUNCTION(NetMulticast, Reliable)
	virtual void OnFireServer(const FVector CameraLocation, const FRotator& SpawnRotation, const FVector& BulletTarget, const TArray<APFPlayerCharacter*>& TargetPlayers);

	UFUNCTION(BlueprintCallable)
	float GetLastFireTime() const { return LastFireTime; }

	// TODO Yanyi Bao
	// Hide
	UFUNCTION(BlueprintCallable)
	void SetLastFireTime(float FireTime) { LastFireTime = FireTime; }

	UFUNCTION(BlueprintCallable)
	float GetFireRate() const { return FireRate; }

	virtual bool CanFire() override;

	UFUNCTION(BlueprintNativeEvent)
	void SetEffectsOnEquiped();

	UFUNCTION(BlueprintNativeEvent)
	void SetEffectsOnDropped();

private:
	UPROPERTY()
	class APFObjectPoolManager* ObjectPoolManager = nullptr;
};
