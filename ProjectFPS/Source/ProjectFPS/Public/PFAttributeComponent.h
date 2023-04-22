// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PFEnums.h"
#include "PFAttributeComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnHealthAttributeChangedDelegate, AActor*, InstigatorActor, UPFAttributeComponent*, OwningComp, float, NewHealth, float, Delta);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnCauseDamageDelegate, AActor*, VictimActor, float, NewHealth, float, Delta);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPropertyModifierChangedDelegate, const TArray<FPropertyModifierDelta>&, Message );


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PROJECTFPS_API UPFAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	static UPFAttributeComponent* GetAttributes(AActor* FromActor);

	UFUNCTION(BlueprintCallable, Category = "Attributes", meta = (DisplayName = "IsAlive"))
	static bool IsActorAlive(AActor* Actor);

	UPFAttributeComponent();

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated, Category = "Attributes")
	float Health;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated, Category = "Attributes")
	float HealthMax;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated, Category = "Attributes")
	float DamageImmuneDuration = 3.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, ReplicatedUsing = OnInvisiblityChange, Category = "Attributes")
	bool Invisiblity = false;

	UFUNCTION(BlueprintNativeEvent)
	void OnInvisiblityChange();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastHealthChanged(AActor* InstigatorActor, float NewHealth, float Delta);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastCauseDamage(AActor* VictimActor, float NewHealth, float Delta);

public:

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	void Restart();

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	bool IsAlive() const;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	bool IsFullHealth() const;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	float GetHealth() const;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	float GetHealthMax() const;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	float GetDamageImmuneDuration() const;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	bool GetVisiblity() const;

	UPROPERTY(BlueprintAssignable, Category = "Attributes")
	FOnHealthAttributeChangedDelegate OnHealthChangedDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Attributes")
	FOnCauseDamageDelegate OnCauseDamageDelegate;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	bool ApplyHealthChange(AActor* InstigatorActor, float Delta);

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	bool ApplyInvisiblityChange(bool Invisible);

	UPROPERTY(BlueprintAssignable, Category = "Attributes")
	FOnPropertyModifierChangedDelegate OnPropertyModifierChangedDelegate;

private:
	TArray<class UPFPerkBaseComponent*> ActivePerkList;

	TMap<EPropertyModifierType, float> PropertyModifierList;

	bool SetPropertyModifier(EPropertyModifierType ModifierType, float ModifierValue, TArray<FPropertyModifierDelta>& PropertyModifierUpdateList);

	UFUNCTION(Client, Unreliable)
	void OnPropertyModifierChanged(const TArray<FPropertyModifierDelta>& PropertyModifierUpdateList);

	void UpdatepPropertyModifier(EPropertyModifierType UpdateTarget = EPropertyModifierType::PMT_All);

public:

	UFUNCTION(BlueprintCallable)
	void RegisterPerk(class UPFPerkBaseComponent* PerkComp);

	UFUNCTION(BlueprintCallable)
	bool UnRegisterPerk(class UPFPerkBaseComponent* PerkComp);

	UFUNCTION(BlueprintCallable)
	void RegisterPerkByType(EPerkType PerkType);

	UFUNCTION(BlueprintCallable)
	bool UnRegisterPerkByType(EPerkType PerkType);

	UFUNCTION(BlueprintCallable)
	float GetPropertyModifier(EPropertyModifierType TargetProperty);
};
