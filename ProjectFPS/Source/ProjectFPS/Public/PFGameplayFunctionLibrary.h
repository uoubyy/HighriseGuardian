// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "Runtime/Online/HTTP/Public/HttpManager.h"
#include "Runtime/Online/HTTP/Public/HttpModule.h"
#include "Runtime/Online/HTTP/Public/HttpRetrySystem.h"
#include "Runtime/Online/HTTP/Public/Interfaces/IHttpRequest.h"
#include "Runtime/Online/HTTP/Public/Interfaces/IHttpResponse.h"

#include "PFEnums.h"

#include "PFGameplayFunctionLibrary.generated.h"
/**
 *
 */
UCLASS()
class PROJECTFPS_API UPFGameplayFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	static bool ApplyDamage(AActor* DamageCauser, AActor* TargetActor, float DamageAmount);

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	static bool AddHealth(AActor* BenefitsCauser, AActor* TargetActor, float HealthAmount);

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	static TArray<APawn*> GetAllTeammates(APawn* Player);

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	static TArray<APawn*> GetAllEnemies(APawn* Player);

	//UFUNCTION(BlueprintCallable, Category = "GamePlay|HTTP")
	static void SendHttpRequestPost(const FString URL, const FString Payload, FHttpRequestCompleteDelegate HttpRequestCompleteDelegate);

	static void SendHttpRequestGet(const FString URL, FHttpRequestCompleteDelegate HttpRequestCompleteDelegate);

	static TArray<TSoftObjectPtr<AActor>> GetAllPickupsOfType(const UWorld* WorldContext, EPowerupType PickupType);

	static TArray<TSoftObjectPtr<AActor>> GetAllPickups(const UWorld* WorldContext);

	static FString ConvertNetModeEnumToString(ENetMode NetMode);

	static FString ConvertNetRoleEnumToString(ENetRole NetRole);
};
