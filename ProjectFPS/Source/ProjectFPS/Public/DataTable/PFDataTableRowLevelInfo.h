// Copyright FPS Team. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "PFDataTableRowLevelInfo.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FPFDataTableRowLevelInfo : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int MapID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UTexture2D* MapThumbnail = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Description;
};
