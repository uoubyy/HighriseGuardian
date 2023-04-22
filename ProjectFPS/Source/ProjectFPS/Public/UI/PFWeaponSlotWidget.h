// Copyright FPS Team. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../PFEnums.h"
#include "PFWeaponSlotWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWeaponSlotSelectedDelegate, EWeaponType, WeaponType, int32, SlotID);

/**
 * 
 */
UCLASS()
class PROJECTFPS_API UPFWeaponSlotWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UImage* SelectedOutLine;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UImage* SlotBackground;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UButton* SlotButton;

	UPROPERTY(BlueprintReadOnly)
	EWeaponType WeaponType;

public:
	UPROPERTY(BlueprintReadOnly)
	int32 SlotID;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void OnSlotSelected();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void OnSlotUnSelected();

	UFUNCTION(BlueprintCallable)
	void InitSlot(EWeaponType ItemType, class UTexture2D* Icon);

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "UI|WeaponSlotWidget")
	FOnWeaponSlotSelectedDelegate OnSlotSelectedDelegate;

	UFUNCTION(BlueprintCallable)
	const EWeaponType GetSlotType() const { return WeaponType; }
};
