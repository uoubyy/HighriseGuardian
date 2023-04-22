// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PFEnums.h"
#include "PFAbilityBaseComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FAbilityTriggerDelegate, AActor*, InstigatorActor, EAbilityType, AbilityType, float, Anticipation, float, Duration, float, CoolDown);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FAbilityStartDelegate, AActor*, InstigatorActor, EAbilityType, AbilityType, float, Duration, float, CoolDown);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FAbilityEndDelegate, AActor*, InstigatorActor, EAbilityType, AbilityType, float, CoolDown);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FAbilityCeasedDelegate, AActor*, InstigatorActor, EAbilityType, AbilityType, float, CoolDown);

UCLASS(Blueprintable, ClassGroup = Component, BlueprintType, meta = (BlueprintSpawnableComponent))
class PROJECTFPS_API UPFAbilityBaseComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UPFAbilityBaseComponent();

	void BeginPlay() override;

	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/** Property replication */
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// If it is affect of time
	UPROPERTY(EditDefaultsOnly)
	bool bIsCharacterSelfAOT;

	// is it being attached
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsBeingAttached;

	// is it in anticipation
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite)
	bool bIsInAnticipation;

	// is it in use
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite)
	bool bIsInUse;

	// Not in cool down
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite)
	bool bIsReady;

	// Ability duration
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	float Duration;

	// Start Delay
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	float Anticipation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool CanCeaseAnticipation;

	// Ability cool down time
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	float CoolDownTime;

	UPROPERTY(BlueprintAssignable, Category = "Ability Events")
	FAbilityTriggerDelegate OnAbilityTrigger;

	UPROPERTY(BlueprintAssignable, Category = "Ability Events")
	FAbilityStartDelegate OnAbilityStart;

	UPROPERTY(BlueprintAssignable, Category = "Ability Events")
	FAbilityEndDelegate OnAbilityEnd;

	UPROPERTY(BlueprintAssignable, Category = "Ability Events")
	FAbilityCeasedDelegate OnAbilityCeased;

	// Trigger Ability
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void CS_TriggerUsing();

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void CS_TriggerCeaseUsing();

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void CS_TriggerCeaseAnticipation();

	UFUNCTION(BlueprintCallable)
	EAbilityType GetAbilityType() const { return AbilityType; }

protected:

	FTimerHandle AnticipationTimer;
	FTimerHandle DurationTimer;
	FTimerHandle CoolDownTimer;

	UPROPERTY(EditAnywhere)
	EAbilityType AbilityType;

	virtual void InitializeComponent() override;

	UFUNCTION(BlueprintImplementableEvent)
	void InitializeAbility();

	//Call C_Anticipation
	UFUNCTION(Client, Reliable)
	void SC_OnAnticipation();

	//presentation on client
	UFUNCTION(BlueprintImplementableEvent)
	void C_OnAnticipation();

	// Effect start
	UFUNCTION()
	void S_OnStartUsing();

	// Effect start details
	UFUNCTION(BlueprintImplementableEvent)
	void S_StartUsing();

	//Call C_OnStartUsing
	UFUNCTION(Client, Reliable)
	void SC_OnStartUsing();

	//presentation on client
	UFUNCTION(BlueprintImplementableEvent)
	void C_OnStartUsing();

	// Effect end
	UFUNCTION()
	void S_OnEndUsing();

	// Effect end details
	UFUNCTION(BlueprintImplementableEvent)
	void S_EndUsing();

	// Call presentation on client
	UFUNCTION(Client, Reliable)
	void SC_OnEndUsing();

	// presentation on client
	UFUNCTION(BlueprintImplementableEvent)
	void C_OnEndUsing();

	// Effect cease
	UFUNCTION()
	void S_OnCeaseUsing();

	// Effect end details
	UFUNCTION(BlueprintImplementableEvent)
	void S_CeaseUsing();

	// Call presentation on client
	UFUNCTION(Client, Reliable)
	void SC_OnCeaseUsing();

	// presentation on client
	UFUNCTION(BlueprintImplementableEvent)
	void C_OnCeaseUsing();

	//reset to ready
	UFUNCTION()
	void S_ResetReady();
};
