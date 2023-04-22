// Copyright FPS Team. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PFHealthModifierInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPFHealthModifierInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PROJECTFPS_API IPFHealthModifierInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintImplementableEvent)
	float S_HealthRatioChangeOnKillEnemy();
};
