// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PFPoolableInterface.h"
#include "PFProjectileBase.generated.h"

UCLASS()
class PROJECTFPS_API APFProjectileBase : public AActor, public IPFPoolableInterface
{
	GENERATED_BODY()

public:
	APFProjectileBase();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Projectile)
	class USphereComponent* CollisionComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Projectile)
	class UStaticMeshComponent* MeshComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	class UProjectileMovementComponent* ProjectileMovement;

	virtual void FellOutOfWorld(const class UDamageType& dmgType) override;

	virtual void OutsideWorldBounds() override;

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Bullet)
	FVector LaunchSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Weapon)
	float DamageAmount;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Weapon)
	float MaxLifeTime;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void Explode();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void ReturnToPool();

public:
	virtual void PostInitializeComponents() override;

	// Poolable Interface Start
	UFUNCTION()
	void OnActive();

	UFUNCTION()
	void OnDeActive();

	UFUNCTION()
	bool IsBussy();

	UFUNCTION()
	void SetObjectPoolManager(class APFObjectPoolManager* ObjectManager);
	// Poolable Interface end

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void SetDamage(const float Damage);

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void InitLaunch(const FVector& NewLocation, FRotator NewRotation);

private:
	FTimerHandle FlyingTimerHandle;

	UFUNCTION()
	void OnDestroyedCallback(AActor* Actor);

	UPROPERTY()
	class APFObjectPoolManager* ObjectPoolManager = nullptr;

	bool IsInUse = false;
};
