// Copyright FPS Team. All Rights Reserved.

#pragma once

#include "Engine/DataTable.h"
#include "PFEnums.h"
#include "PFDataTableRowWeapon.generated.h"

USTRUCT(BlueprintType)
struct FPFDataTableRowWeapon : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EWeaponType WeaponType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UTexture2D* WeaponIcon = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UTexture2D* KillRecordWeaponIcon = nullptr;
	
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
	FString Description;

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
	int maxAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UTexture2D* IndicatorIcon = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Epuipable = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UAnimMontage* OnFireAnim = nullptr;
};