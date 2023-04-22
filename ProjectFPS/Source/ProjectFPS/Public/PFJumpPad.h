// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PFJumpPad.generated.h"

UCLASS()
class PROJECTFPS_API APFJumpPad : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APFJumpPad();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USceneComponent* SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UBoxComponent* TriggerBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UParticleSystemComponent* PadParticle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UPointLightComponent* PointLight1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UStaticMeshComponent* SM_Indicator;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UArrowComponent* Arrow;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump Pad", meta = (MakeEditWidget = true))
	FVector JumpTarget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump Pad")
	float JumpTime;

private:

	UPROPERTY()
	FVector TargetPosition; // Target position in world space

	UPROPERTY()
	FVector LaunchVelocity;

	UFUNCTION()
	void OnJumpPadBeginOverlap(class UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);


public:

	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;

	UFUNCTION(BlueprintImplementableEvent)
	void Launch(AActor* Actor);
};
