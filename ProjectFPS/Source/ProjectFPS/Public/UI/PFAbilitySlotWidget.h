// Copyright FPS Team. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../PFEnums.h"
#include "PFAbilitySlotWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAbilitySlotSelectedDelegate, EAbilityType, AbilityType, int32, SlotID);

/**
 * 
 */
UCLASS()
class PROJECTFPS_API UPFAbilitySlotWidget : public UUserWidget
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
	EAbilityType AbilityType;

public:
	UPROPERTY(BlueprintReadOnly)
	int32 SlotID;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void OnSlotSelected();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void OnSlotUnSelected();

	UFUNCTION(BlueprintCallable)
	void InitSlot(EAbilityType ItemType, class UTexture2D* Icon);

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "UI|AbilitySlotWidget")
	FOnAbilitySlotSelectedDelegate OnSlotSelectedDelegate;

	UFUNCTION(BlueprintCallable)
	const EAbilityType GetSlotType() const { return AbilityType; }
	
};
