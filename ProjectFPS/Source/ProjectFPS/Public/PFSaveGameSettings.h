// Copyright FPS Team. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "PFSaveGameSettings.generated.h"

/**
 * 
 */
UCLASS(Config = Game, defaultconfig, meta = (DisplayName = "Save Game Settings"))
class PROJECTFPS_API UPFSaveGameSettings : public UDeveloperSettings
{
	GENERATED_BODY()
	
public:
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "General")
	FString NakamaHost;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "General")
	int32 NakamaPort;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "General")
	FString NakamaDebugUniqueID;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "General")
	int32 NakamaPlayerNums;
};
