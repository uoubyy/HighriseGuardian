// Copyright FPS Team. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PFPerkBaseComponent.generated.h"


UCLASS(Blueprintable, ClassGroup = Component, BlueprintType, meta = (BlueprintSpawnableComponent))
class PROJECTFPS_API UPFPerkBaseComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly)
	bool bIsPermanent = false;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite)
	bool bIsPerkActive = false;

public:	
	// Sets default values for this component's properties
	UPFPerkBaseComponent();

	UFUNCTION(BlueprintCallable)
	bool IsPerkActive() const { return bIsPerkActive; }

	UFUNCTION(BlueprintCallable)
	bool IsPermanentPerk() const { return bIsPermanent; }

	UFUNCTION(BlueprintCallable)
	void S_TriggerActivate();

	UFUNCTION(BlueprintNativeEvent)
	void S_ActivateForRelatedSystem();

	UFUNCTION(BlueprintNativeEvent)
	void S_ActiveEffectOnServer();

	UFUNCTION(Client, Reliable)
	void SC_ActiveEffectOnPlayerClient();

	UFUNCTION(BlueprintNativeEvent)
	void C_ActiveEffectOnPlayerClient();

	UFUNCTION(NetMulticast, Reliable)
	void SC_ActiveEffectOnOthersClient();

	UFUNCTION(BlueprintNativeEvent)
	void C_ActiveEffectOnOthersClient();

	UFUNCTION(BlueprintCallable)
	void S_TriggerDeactivate();

	UFUNCTION(BlueprintNativeEvent)
	void S_DeactivateForRelatedSystem();

	UFUNCTION(BlueprintNativeEvent)
	void S_EndEffectOnServer();

	UFUNCTION(Client, Reliable)
	void SC_EndEffectOnPlayerClient();

	UFUNCTION(BlueprintNativeEvent)
	void C_EndEffectOnPlayerClient();

	UFUNCTION(NetMulticast, Reliable)
	void SC_EndEffectOnOthersClient();

	UFUNCTION(BlueprintNativeEvent)
	void C_EndEffectOnOthersClient();

	UFUNCTION(BlueprintCallable)
	FString GetPerkName() const { return PerkName; }

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FString PerkName;

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void InitializeComponent() override;

	virtual void UninitializeComponent() override;

	UFUNCTION(BlueprintNativeEvent)
	void InitializePerk();

	UFUNCTION(BlueprintNativeEvent)
	void UninitializePerk();

	// every time update value mark as dirty
	bool DityMark = false;

	UFUNCTION(BlueprintCallable)
	void MarkAsDirty() { DityMark = true; }

	UFUNCTION(BlueprintCallable)
	void CleanDirtyMark() { DityMark = false; }

public:
	bool IsDirty() const { return DityMark; }

};
