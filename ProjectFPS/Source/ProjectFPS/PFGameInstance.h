// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "PFGameInstance.generated.h"

/**
 *
 */
UCLASS()
class PROJECTFPS_API UPFGameInstance : public UGameInstance {
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, Category = PlayerInfo)
	FString PlayerDisplayName;
};
