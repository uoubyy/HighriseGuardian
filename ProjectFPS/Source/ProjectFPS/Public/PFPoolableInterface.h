// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PFPoolableInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPFPoolableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 *
 */
class PROJECTFPS_API IPFPoolableInterface
{
	GENERATED_BODY()

		// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION()
	virtual void OnActive() = 0;

	UFUNCTION()
	virtual void OnDeActive() = 0;

	UFUNCTION()
	virtual bool IsBussy() = 0;

	UFUNCTION()
	virtual void SetObjectPoolManager(class APFObjectPoolManager* ObjectManager) = 0;
};
