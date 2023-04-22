// Copyright FPS Team. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NiagaraComponent.h"
#include "PFPortal.generated.h"

UCLASS()
class PROJECTFPS_API APFPortal : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APFPortal();

protected:
	virtual void PostInitializeComponents() override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		class USceneComponent* SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		class UNiagaraComponent* Niagara;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		class UBoxComponent* TriggerBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Portal", meta = (MakeEditWidget = true))
		FTransform TeleportTarget;

private:
	UFUNCTION()
		void OnPortalBeginOverlap(class UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
