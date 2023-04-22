// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"

#include "PFPlayerCharacter.h"
#include "PFPlayerController.generated.h"

/**
 *
 */
UCLASS()
class PROJECTFPS_API APFPlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SpawnPlayerCharacter(TSubclassOf<APFPlayerCharacter> CharacterClass, FTransform SpawnTransform, FRotator SpawnRotation);

	virtual void OnRep_PlayerState() override;

	virtual void GameHasEnded( class AActor* EndGameFocus = nullptr, bool bIsWinner = false ) override;

protected:

	virtual void BeginPlay() override;

	virtual void OnPossess(APawn* aPawn) override;

	virtual void OnUnPossess() override;

	UFUNCTION(Reliable, Client)
	void PostExitMatch(bool bIsWinner);

public:

	//set rotation from server
	UFUNCTION(Client, Reliable, BlueprintCallable)
	void SetControlRotationFromServer(const FRotator& NewRotation);

	UFUNCTION(Server, Reliable)
	void SetPlayerDisplayName(const FString& NewName);

	UFUNCTION()
	void OnRePossessOldCharacter();

	UFUNCTION()
	void SetPlayerSessionID(FString SessionID) { PlayerSessionID = SessionID; }

	UFUNCTION()
	FString GetPlayerSessionID();

private:
	TWeakObjectPtr<APawn> OldCharacter;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FString PlayerSessionID;
};
