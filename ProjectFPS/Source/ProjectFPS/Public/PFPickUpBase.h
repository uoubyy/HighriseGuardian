// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PFEnums.h"
#include "PFPickUpBase.generated.h"

UCLASS()
class PROJECTFPS_API APFPickUpBase : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APFPickUpBase();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USceneComponent* SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UBoxComponent* TriggerBox;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Power Up")
	EPowerupType PowerupType;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Power Up")
	float BenefitsAmount;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Power Up")
	EWeaponType TargetweaponType;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Power Up")
	int32 AmmoBenefits;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Power Up")
	float RespawnTime = 15.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Power UP")
	float RespawnCDLeftTime;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Power UP")
	bool IsPickupAvailable;

	virtual void PostInitializeComponents() override;

	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(NetMulticast, Reliable)
	void OnItemConsumed();

	UFUNCTION(BlueprintImplementableEvent)
	void OnItemConsumedClient();

	UFUNCTION(NetMulticast, Reliable)
	void OnItemActived();

	UFUNCTION(BlueprintImplementableEvent)
	void OnItemActivedClient();

private:
	UFUNCTION()
	void OnPortalBeginOverlap(class UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY()
	FTimerHandle TimerHandle_Respawn;

public:

	UFUNCTION(BlueprintCallable)
	EPowerupType GetPowerupType() const { return PowerupType; }

	UFUNCTION(BlueprintCallable)
	EWeaponType GetTargetWeaponType() const;
};
