// Copyright FPS Team. All Rights Reserved.

#pragma once

#include "Engine/DataTable.h"
#include "PFEnums.h"
#include "PFDataTableRowPerk.generated.h"

USTRUCT(BlueprintType)
struct FPFDataTableRowPerk : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EPerkType PerkType;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UTexture2D* PerkIcon = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Epuipable = true;
};