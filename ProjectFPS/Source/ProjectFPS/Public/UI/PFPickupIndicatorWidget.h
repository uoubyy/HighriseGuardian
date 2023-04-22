// Copyright FPS Team. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PFEnums.h"

#include "PFPickupIndicatorWidget.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTFPS_API UPFPickupIndicatorWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UBorder* PickupIndicator;

	UPROPERTY(BlueprintReadWrite)
	EPowerupType PickupType;

	UPROPERTY(BlueprintReadOnly)
	TWeakObjectPtr<AActor> PickupActor;

	bool IsActive = false;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	virtual void BeginDestroy() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UDataTable* WeaponsInfoDataTable;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UTexture2D* HealthBoxIcon;

public:
	UFUNCTION(BlueprintCallable)
	void InitPickupIndicator(AActor* TargetActor, EPowerupType TargetType);

	UFUNCTION(BlueprintCallable)
	EPowerupType GetPickupIndicatorTargetType();

	UFUNCTION(BlueprintCallable)
	EWeaponType GetPickupIndicatorTargetWeaponType();

	UFUNCTION(BlueprintCallable)
	void TogglePickupIndicator(bool Value);
};
