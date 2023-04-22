// Copyright FPS Team. All Rights Reserved.


#include "PFObjectPoolManager.h"
#include "PFPoolableInterface.h"

// Sets default values
APFObjectPoolManager::APFObjectPoolManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
}

void APFObjectPoolManager::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if(HasAuthority())
	{ 
		for (auto ObjectClass : PoolableObjectClass)
		{
			RegisterPoolableClass(ObjectClass, 10);
		}
	}
}

void APFObjectPoolManager::RegisterPoolableClass(UClass* ObjectClass, int InitialNum)
{
	if (!ObjectClass->ImplementsInterface(UPFPoolableInterface::StaticClass()))
	{
		UE_LOG(LogTemp, Warning, TEXT("Object Type %s hasn't Implement Poolable Interface!"), *ObjectClass->GetName());
		return;
	}

	for (const auto& ObjectList : ObjectPool)
	{
		if (ObjectList.PoolableObjectClassName == ObjectClass->GetName())
		{
			UE_LOG(LogTemp, Warning, TEXT("Poolable Object Class %s has registed!"), *ObjectClass->GetName());
			return;
		}
	}

	FPoolableObjectList ObjectList;
	ObjectList.PoolableObjectClassName = ObjectClass->GetName();

	FActorSpawnParameters* SpawnParameters = new FActorSpawnParameters;
	SpawnParameters->SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	for (int i = 0; i < InitialNum; ++i)
	{
		AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(ObjectClass, FVector::ZeroVector, FRotator::ZeroRotator, *SpawnParameters);

		if (IsValid(SpawnedActor))
		{
			IPFPoolableInterface* PoolableInterface = Cast<IPFPoolableInterface>(SpawnedActor);
			PoolableInterface->SetObjectPoolManager(this);
			PoolableInterface->OnDeActive();

			ObjectList.PoolableObjects.Push(SpawnedActor);
		}
	}

	ObjectPool.Add(ObjectList);

	UE_LOG(LogTemp, Warning, TEXT("Register Poolable Class %s."), *ObjectClass->GetName());
}

AActor* APFObjectPoolManager::GetObjectOfType(UClass* ObjectClass)
{
	AActor* PoolableObject = nullptr;
	for (auto& ObjectList : ObjectPool)
	{
		if (ObjectList.PoolableObjectClassName == ObjectClass->GetName())
		{
			for (auto IdleObject : ObjectList.PoolableObjects)
			{
				if (IdleObject == nullptr)
				{
					UE_LOG(LogTemp, Warning, TEXT("Object of type %s in object pool is invalid"), *ObjectClass->GetName());
				}

				IPFPoolableInterface* PoolableInterface = Cast<IPFPoolableInterface>(IdleObject);
				if (IdleObject && !PoolableInterface->IsBussy())
				{
					PoolableObject = IdleObject;
					break;
				}
			}

			if (!PoolableObject)
			{
				FActorSpawnParameters* SpawnParameters = new FActorSpawnParameters;
				SpawnParameters->SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
				PoolableObject = GetWorld()->SpawnActor<AActor>(ObjectClass, FVector::ZeroVector, FRotator::ZeroRotator, *SpawnParameters);

				IPFPoolableInterface* PoolableInterface = Cast<IPFPoolableInterface>(PoolableObject);
				PoolableInterface->SetObjectPoolManager(this);
				ObjectList.PoolableObjects.Push(PoolableObject);
			}
		}
	}

	if(!PoolableObject)
		UE_LOG(LogTemp, Warning, TEXT("Object of type %s not exists in object pool"), *ObjectClass->GetName());

	return PoolableObject;
}

bool APFObjectPoolManager::ReturnToPool(AActor* PoolableObject)
{
	UClass* ObjectClass = PoolableObject->GetClass();

	IPFPoolableInterface* PoolableInterface = Cast<IPFPoolableInterface>(PoolableObject);
	if (!PoolableInterface)
	{
		UE_LOG(LogTemp, Warning, TEXT("Return Un-Poolable Object to Object Pool!"));
		return false;
	}
	PoolableInterface->OnDeActive();

	for (auto& ObjectList : ObjectPool)
	{
		if (ObjectList.PoolableObjectClassName == ObjectClass->GetName())
		{
			for (auto IdleObject : ObjectList.PoolableObjects)
			{
				if (IdleObject == PoolableObject)
					return true;
			}
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("Return Poolable Object %s to Object Pool Failed!"), *PoolableObject->GetName());

	return false;
}
