// Copyright FPS Team. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PFObjectPoolManager.generated.h"

USTRUCT()
struct FPoolableObjectList
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FString PoolableObjectClassName;

	UPROPERTY()
	TArray<AActor*> PoolableObjects;
};

UCLASS()
class PROJECTFPS_API APFObjectPoolManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APFObjectPoolManager();

	virtual void PostInitializeComponents() override;

	void RegisterPoolableClass(UClass* ObjectClass, int InitialNum);

	UFUNCTION(BlueprintCallable)
	AActor* GetObjectOfType(UClass* ObjectClass);

	bool ReturnToPool(AActor* PoolableObject);

	UPROPERTY(EditDefaultsOnly, Category = "Object Pool Manager")
	TArray<TSubclassOf<AActor>> PoolableObjectClass;

private:
	UPROPERTY()
	TArray<FPoolableObjectList> ObjectPool;
};
