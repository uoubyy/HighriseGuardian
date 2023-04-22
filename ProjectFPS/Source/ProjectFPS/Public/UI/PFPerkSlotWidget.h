// Copyright FPS Team. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../PFEnums.h"
#include "PFPerkSlotWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPerkSlotSelectedDelegate, EPerkType, PerkType, int32, SlotID);

/**
 *
 */
UCLASS()
class PROJECTFPS_API UPFPerkSlotWidget : public UUserWidget
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
		EPerkType PerkType;

public:
	UPROPERTY(BlueprintReadOnly)
		int32 SlotID;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void OnSlotSelected();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void OnSlotUnSelected();

	UFUNCTION(BlueprintCallable)
		void InitSlot(EPerkType ItemType, class UTexture2D* Icon);

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "UI|PerkSlotWidget")
		FOnPerkSlotSelectedDelegate OnSlotSelectedDelegate;

	UFUNCTION(BlueprintCallable)
		const EPerkType GetSlotType() const { return PerkType; }

};
