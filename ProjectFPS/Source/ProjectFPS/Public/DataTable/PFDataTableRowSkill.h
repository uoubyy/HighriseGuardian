// Copyright FPS Team. All Rights Reserved.

#pragma once

#include "Engine/DataTable.h"
#include "PFEnums.h"
#include "PFDataTableRowSkill.generated.h"

USTRUCT(BlueprintType)
struct FPFDataTableRowSkill : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EAbilityType AbilityType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UTexture2D* AbilityIcon = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Epuipable = true;
};