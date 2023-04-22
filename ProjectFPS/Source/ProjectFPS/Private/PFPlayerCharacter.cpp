// Fill out your copyright notice in the Description page of Project Settings.


#include "PFPlayerCharacter.h"
#include <Camera/CameraComponent.h>
#include "PFWeaponBase.h"
#include "PFWeaponBaseClient.h"
#include "PFWeaponBaseServer.h"
#include "PFPlayerController.h"
#include "PFAttributeComponent.h"

#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/GameMode.h"

#include "Net/UnrealNetwork.h"
#include "PFPlayerState.h"

#include <Blueprint/UserWidget.h>
#include "UI/PFInGameHUD.h"
#include "UI/PFScoreboardWidget.h"

#include "../PFGameStateBase.h"
#include "../PFGameInstance.h"

#include "PFAbilityBaseComponent.h"
#include "Components/TextRenderComponent.h"

#include "PFGameplayFunctionLibrary.h"
#include "PFNakamaSubsystem.h"
#include "Camera/CameraActor.h"
#include "PFPlayerState.h"
#include <PFPerkBaseComponent.h>
#include <../Plugins/FX/Niagara/Source/Niagara/Public/NiagaraComponent.h>

// Sets default values
APFPlayerCharacter::APFPlayerCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	PlayerCamera = CreateDefaultSubobject<UCameraComponent>(FName("PlayerCamera"));
	PlayerCamera->SetupAttachment(RootComponent);

	FPMesh = CreateDefaultSubobject<USkeletalMeshComponent>(FName("FirstPersonMesh"));
	FPMesh->SetupAttachment(PlayerCamera);
	FPMesh->SetOnlyOwnerSee(true);

	PlayerOutlineMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("OutlineMesh"));
	PlayerOutlineMesh->SetupAttachment(RootComponent);

	AttributeComp = CreateDefaultSubobject<UPFAttributeComponent>("AttributeComp");

	bReplicates = true;

	bInputReverse = false;
}

void APFPlayerCharacter::Restart()
{
	Super::Restart();

	EnterDamageImmune();

	// NetMode == NM_Client && GetLocalRole() == ROLE_AutonomousProxy
	// Local PlayerController Possessed Pawn
	IsLocalPlayer = Controller->IsLocalController();

	// UI Event start
	if (!IsInitComplete && IsLocalPlayer)
	{
		UE_LOG(LogTemp, Warning, TEXT("PFPlayerCharacter::Restart"));
		AttributeComp->OnHealthChangedDelegate.AddDynamic(this, &APFPlayerCharacter::OnHealthChanged);
		AttributeComp->OnCauseDamageDelegate.AddDynamic(this, &APFPlayerCharacter::OnCauseDamage);

		APFPlayerState* PFPlayerState = Controller->GetPlayerState<APFPlayerState>();
		RefreshInGameHUD(PFPlayerState);

		APFGameStateBase* PFGameState = GetWorld() != NULL ? GetWorld()->GetGameState<APFGameStateBase>() : NULL;
		if (PFGameState)
		{
			PFGameState->OnMatchStateChange.AddDynamic(this, &APFPlayerCharacter::OnMatchStateChange);
			OnMatchStateChange(PFGameState->GetMatchState(), FName("None"));

			UE_LOG(LogTemp, Warning, TEXT("OnRep_StageCountDownChange Event Register %d"), PFGameState->GetMatchStageCountDown());
			PFGameState->OnMatchStageCountDownChange.AddDynamic(this, &APFPlayerCharacter::OnMatchStageCountDownChange);
			OnMatchStageCountDownChange(PFGameState->GetMatchStageCountDown());

			PFGameState->OnMatchScoreChange.AddDynamic(this, &APFPlayerCharacter::OnMatchScoreChange);

			PFGameState->OnKillRecordReportedDelegate.AddDynamic(InGameHUD, &UPFInGameHUD::OnKillRecordReported);
		}
	}
	// UI Event end

	StopFire();

	OnCharacterInitComplete();

	APFPlayerState* PFPlayerState = Controller->GetPlayerState<APFPlayerState>();
	FString PlayerName = PFPlayerState ? PFPlayerState->GetPlayerName() : "NONAME";
	FString HasAuthorityStr = HasAuthority() ? "True" : "False";
	FString NetModeStr = UPFGameplayFunctionLibrary::ConvertNetModeEnumToString(GetNetMode());
	FString LocalRoleStr = UPFGameplayFunctionLibrary::ConvertNetRoleEnumToString(GetLocalRole());
	FString RemoteRoleStr = UPFGameplayFunctionLibrary::ConvertNetRoleEnumToString(GetRemoteRole());
	FString IsLocalPlayerStr = IsLocalPlayer ? "True" : "False";

	UE_LOG(LogTemp, Warning, TEXT("On Player Restart %s, Net Mode: %s, LocalRole: %s, RemoteRole: %s, HasAuthority: %s, IsLocalPlayer %s."), *PlayerName, *NetModeStr, *LocalRoleStr, *RemoteRoleStr, *HasAuthorityStr, *IsLocalPlayerStr);
}

void APFPlayerCharacter::PawnClientRestart()
{
	Super::PawnClientRestart();
	RequestSpawnEntranceWeapon();
}

void APFPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	APFPlayerState* PFPlayerState = NewController->GetPlayerState<APFPlayerState>();
	if (PFPlayerState)
	{
		OnTeamChanged(PFPlayerState->GetTeamID());

		// Player Character in the Dedicated Server
		if (!IsInitComplete && GetNetMode() == ENetMode::NM_DedicatedServer)
		{
			// Dynamic attach perk component
			for (auto PerkType : PFPlayerState->GetInventoryPerks())
			{
				ensureMsgf(PerkComponentClasses.Contains(PerkType), TEXT("Can not find Perk Component Class!"));
				UPFPerkBaseComponent* PerkComp = NewObject<UPFPerkBaseComponent>(this, PerkComponentClasses[PerkType]);
				if (PerkComp)
				{
					PerkComp->RegisterComponent();
					AddInstanceComponent(PerkComp);
				}
			}

			// Dynamic attach ability component
			for (int32 SlotID = 1; SlotID <= PFPlayerState->GetInventoryAbilities().Num(); SlotID++)
			{
				auto AbilityType = PFPlayerState->GetAbilityTypeOfSlot(SlotID);
				ensureMsgf(AbilityComponentClasses.Contains(AbilityType), TEXT("Can not find Ability Component Class!"));
				UPFAbilityBaseComponent* AbilityComp = NewObject<UPFAbilityBaseComponent>(this, AbilityComponentClasses[AbilityType]);
				if (AbilityComp)
				{
					AbilityComp->RegisterComponent();
					AddInstanceComponent(AbilityComp);

					AbilityComponents.Add(AbilityComp);
				}
			}
			OnCharacterInitComplete();
		}
	}
}

void APFPlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	APFPlayerState* PFPlayerState = GetPlayerState<APFPlayerState>();
	if (PFPlayerState)
	{
		OnTeamChanged(PFPlayerState->GetTeamID());
		APlayerController* LocalPlayerController = GetWorld()->GetFirstPlayerController();
		ULocalPlayer* LocalPlayer = LocalPlayerController ? LocalPlayerController->GetLocalPlayer() : nullptr;
		if (LocalPlayerController)
		{
			UPFNakamaSubsystem* PFNakamaSubsystem = LocalPlayer->GetSubsystem<UPFNakamaSubsystem>();
			if (PFNakamaSubsystem)
			{
				LocalPlayerTeamID = PFNakamaSubsystem->GetLocalPlayerTeamID();

				bool IsTeammate = PFPlayerState->GetTeamID() == LocalPlayerTeamID;

				this->ChangePlayerOutlineMeshColor(IsTeammate);
			}
		}

		if (GetLocalRole() == ROLE_AutonomousProxy)
		{
			RefreshInGameHUD(PFPlayerState);
		}

		// Simulated Pawn
		// Other Players in my client
		if (!IsInitComplete && GetLocalRole() == ENetRole::ROLE_SimulatedProxy)
		{
			OnCharacterInitComplete();
		}

		FString PlayerName = PFPlayerState->GetPlayerName();
		FString HasAuthorityStr = HasAuthority() ? "True" : "False";
		FString NetModeStr = UPFGameplayFunctionLibrary::ConvertNetModeEnumToString(GetNetMode());
		FString LocalRoleStr = UPFGameplayFunctionLibrary::ConvertNetRoleEnumToString(GetLocalRole());
		FString RemoteRoleStr = UPFGameplayFunctionLibrary::ConvertNetRoleEnumToString(GetRemoteRole());

		UE_LOG(LogTemp, Warning, TEXT("OnRep_PlayerState Player %s, Net Mode: %s, LocalRole: %s, RemoteRole: %s, HasAuthority: %s."), *PlayerName, *NetModeStr, *LocalRoleStr, *RemoteRoleStr, *HasAuthorityStr);
	}
}


void APFPlayerCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	InitCharacterBlueprintComponent();
}

// Get Initial weapon type from server
void APFPlayerCharacter::RequestSpawnEntranceWeapon_Implementation(/*EWeaponType WeaponType*/)
{
	APFPlayerState* PFPlayerState = GetPlayerState<APFPlayerState>();

	if (!PFPlayerState) return;
	// ensureMsgf(CurrentWeaponServer == nullptr, TEXT("Exists weapon server before init!"));
	EWeaponType WeaponType = PFPlayerState->GetInitialWeaponType();

	if (WeaponType != EWeaponType::WT_None)
	{

		APFWeaponBaseServer* NewWeapon = Cast<APFWeaponBaseServer>(APFWeaponBase::SpawnWeapon(GetWorld(), ServerWeaponClasses[WeaponType], GetActorTransform(), this));
		NewWeapon->SetOwner(this);
		WeaponListServer.Add(WeaponType, NewWeapon);

		CurrentWeaponServer = NewWeapon;
		CurrentWeaponServer->OnWeaponEquiped(GetMesh(), TEXT("WeaponSocket"));
	}
}

bool APFPlayerCharacter::RequestSpawnEntranceWeapon_Validate(/*EWeaponType WeaponType*/)
{
	return true;
}

void APFPlayerCharacter::OnHealthChanged(AActor* InstigatorActor, UPFAttributeComponent* OwningComp, float NewHealth, float Delta)
{
	// InstigatorActor: Damage Causer of Heal Causer
	APFPlayerController* PC = Cast<APFPlayerController>(GetController());

	// Died
	if (NewHealth <= 0.0f && Delta < 0.0f)
	{
		DisableInput(PC);
	}
}

void APFPlayerCharacter::OnCauseDamage(AActor* VictimActor, float NewHealth, float Delta)
{
	Cast<APFPlayerCharacter>(VictimActor)->UpdateHealthBar(NewHealth, Delta);
}

void APFPlayerCharacter::OnMatchStateChange(FName NewState, FName OldState)
{
	APFPlayerController* PC = Cast<APFPlayerController>(GetController());
	if (NewState == MatchState::InProgress)
		EnableInput(PC);
	else
		DisableInput(PC);

	// UI Event start
	if (IsLocalPlayer && NewState == MatchState::WaitingPostMatch)
	{
		InGameHUD->RemoveFromParent();
		Scoreboard = Scoreboard ? Scoreboard : Cast<UPFScoreboardWidget>(CreateWidget(GetWorld(), ScoreboardClass));

		APFGameStateBase* GameStateBase = GetWorld() != NULL ? GetWorld()->GetGameState<APFGameStateBase>() : NULL;
		if (GameStateBase)
		{
			FPFMatchResult MatchResult = GameStateBase->GetMatchResult();
			TArray<APFPlayerState*> TeamInfo1;
			TArray<APFPlayerState*> TeamInfo2;
			const TArray<TObjectPtr<APlayerState>> playerStates = GameStateBase->PlayerArray;
			for (auto playerState : playerStates) {
				APFPlayerState* state = Cast<APFPlayerState>(playerState);
				if (state->GetTeamID() == 0) {
					TeamInfo1.Add(state);
				}
				else if (state->GetTeamID() == 1) {
					TeamInfo2.Add(state);
				}
			}

			EMatchResult MatchResultState = EMatchResult::MR_Draw;
			if(LocalPlayerTeamID == 0 && MatchResult.ScoreTeam1 > MatchResult.ScoreTeam2)
				MatchResultState = EMatchResult::MR_Won;
			else if (LocalPlayerTeamID == 0 && MatchResult.ScoreTeam1 < MatchResult.ScoreTeam2)
				MatchResultState = EMatchResult::MR_Lose;

			Scoreboard->InitScoreboard(MatchResult.ScoreListTeam1, MatchResult.ScoreListTeam2, MatchResult.ScoreTeam1, MatchResult.ScoreTeam2, TeamInfo1, TeamInfo2, MatchResultState);
		}

		// Scoreboard->AddToViewport();
		if(!Scoreboard->IsInViewport())
			Scoreboard->ToggleScoreboard(true);
	}

	if (IsLocalPlayer && NewState == MatchState::InProgress)
	{
		InGameHUD->OnMatchStateChanged(NewState);
	}
	// UI Event end
}

void APFPlayerCharacter::OnMatchScoreChange(int32 TeamAScore, int32 TeamBScore)
{
	// When Match Score changed, this player may has been killed
	// that means No Controller Possessed and means no player state

	ensureMsgf(InGameHUD, TEXT("No In Game HUD Found"));
	{
		int32 LeftScore = TeamID == 0 ? TeamAScore : TeamBScore;
		int32 RightScore = TeamID == 0 ? TeamBScore : TeamAScore;
		InGameHUD->OnMatchScoreChanged(LeftScore, RightScore);
	}
}

void APFPlayerCharacter::OnTeamChanged(int Team)
{
	TeamID = Team;
}

bool APFPlayerCharacter::GetIsLocalPlayer() const
{
	if(IsLocalPlayer) return true;

	return (GetNetMode() == ENetMode::NM_Client && GetLocalRole() == ROLE_AutonomousProxy) || (GetNetMode() == ENetMode::NM_Standalone);
}

void APFPlayerCharacter::FellOutOfWorld(const class UDamageType& dmgType)
{
	if (HasAuthority())
	{
		UPFGameplayFunctionLibrary::ApplyDamage(this, this, AttributeComp->GetHealthMax());
	}
}

void APFPlayerCharacter::RequestChangeMousesenSitivity(float Value)
{
	MouseSensitivity = Value;
	ChangeMousesenSitivity(Value);
}

void APFPlayerCharacter::ChangeMousesenSitivity_Implementation(float Value)
{
	MouseSensitivity = Value;
}

bool APFPlayerCharacter::ChangeMousesenSitivity_Validate(float Value)
{
	return true;
}

void APFPlayerCharacter::ToggleSettingsPanel()
{
	if (InGameHUD)
	{
		InGameHUD->ToggleSettingsPanel();
	}
}

void APFPlayerCharacter::RefreshInGameHUD(APFPlayerState* PFPlayerState)
{
	if (!InGameHUD)
	{
		InGameHUD = Cast<UPFInGameHUD>(CreateWidget(GetWorld(), InGameHUDClass));
		InGameHUD->AddToViewport();

		InGameHUD->InitInGameHUD(AttributeComp);
	}

	InGameHUD->RefreshInGameHUD(PFPlayerState);
}

void APFPlayerCharacter::OnMatchStageCountDownChange(int CountDown)
{
	UE_LOG(LogTemp, Warning, TEXT("OnMatchStageCountDownChange %d."), CountDown);
	if (InGameHUD)
	{
		InGameHUD->OnCountDownChanged(CountDown);
	}
}

// Called to bind functionality to input
void APFPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &APFPlayerCharacter::StartFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &APFPlayerCharacter::StopFire);

	PlayerInputComponent->BindAction("NextWeapon", IE_Pressed, this, &APFPlayerCharacter::NextWeapon);
	PlayerInputComponent->BindAction("LastWeapon", IE_Pressed, this, &APFPlayerCharacter::LastWeapon);

	PlayerInputComponent->BindAction<FSwitchWeaponDelegate>("Weapon1", IE_Pressed, this, &APFPlayerCharacter::RequestSwitchWeaponWithID, 1);
	PlayerInputComponent->BindAction<FSwitchWeaponDelegate>("Weapon2", IE_Pressed, this, &APFPlayerCharacter::RequestSwitchWeaponWithID, 2);
	PlayerInputComponent->BindAction<FSwitchWeaponDelegate>("Weapon3", IE_Pressed, this, &APFPlayerCharacter::RequestSwitchWeaponWithID, 3);
	PlayerInputComponent->BindAction<FSwitchWeaponDelegate>("Weapon4", IE_Pressed, this, &APFPlayerCharacter::RequestSwitchWeaponWithID, 4);

	PlayerInputComponent->BindAction("ShowScoreBoard", IE_Pressed, this, &APFPlayerCharacter::ShowScoreBoard);
	PlayerInputComponent->BindAction("ToggleSettingsPanel", IE_Pressed, this, &APFPlayerCharacter::ToggleSettingsPanel);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &APFPlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APFPlayerCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APFPlayerCharacter::ViewLookRight);
	PlayerInputComponent->BindAxis("LookUp", this, &APFPlayerCharacter::ViewLookUp);

}

void APFPlayerCharacter::OnFire()
{
	// Fire is triggered by client
	// The client can fire immediately
	// The real fire will be called in server
	if (!CurrentWeaponClient)
	{
		UE_LOG(LogTemp, Warning, TEXT("OnFire Failed, CurrentWeaponClient is null!"));
		return;
	}
	if (!CurrentWeaponServer)
	{
		UE_LOG(LogTemp, Warning, TEXT("OnFire Failed, CurrentWeaponServer is null!"));
		return;
	}
	
	//BulletTargetPoint is used to record line trace result(Bullet Weapon Only)
	FVector BulletTargetPoint(0, 0, 0);
	//TargetPlayer is usesd to record line hit character(Line Trace Weapon Only)
	TArray<APFPlayerCharacter*> TargetPlayers;

	//Client will return the line track result
	ClientFire(BulletTargetPoint, TargetPlayers);

	//Transmit it to server
	ServerFire(BulletTargetPoint, TargetPlayers);

	UpdateAmmoUI();

	if (CurrentWeaponServer->GetCurrAmmo() <= 0)
		StopFire();
	
	// TODO Yanyi Bao
	// Encapsule to weapon class
	//If not, update last fire time
	CurrentWeaponServer->SetLastFireTime(UGameplayStatics::GetRealTimeSeconds(GetWorld()));
}

void APFPlayerCharacter::StartFire()
{
	if (CurrentWeaponClient && CurrentWeaponClient->CanFire())
	{
		IsFiring = true;
		OnIsFiringDelegate.Broadcast();
		OnFire();
		if (CurrentWeaponClient->IsAutoWeapon() && CurrentWeaponClient->GetCurrAmmo() > 0)// If current weapon is auto weapon, set fire timer
		{
			GetWorldTimerManager().SetTimer(FireHandle, this, &APFPlayerCharacter::OnFire, 1 / CurrentWeaponServer->GetFireRate(), true);
		}
	}
}

void APFPlayerCharacter::StopFire()
{
	IsFiring = false;
	GetWorldTimerManager().ClearTimer(FireHandle);
}

void APFPlayerCharacter::OnReLoad()
{
	if (CurrentWeaponClient)
		CurrentWeaponClient->OnReLoad();
	ReLoadServer();
	UpdateAmmoUI();
}

void APFPlayerCharacter::NextWeapon()
{
	APFPlayerState* PFPlayerState = GetPlayerState<APFPlayerState>();
	if(PFPlayerState)
		RequestSwitchWeaponClient(PFPlayerState->GetNextValidWeaponType());
}

void APFPlayerCharacter::LastWeapon()
{
	APFPlayerState* PFPlayerState = GetPlayerState<APFPlayerState>();
	int LastWeaponID = PFPlayerState->GetSlotOfWeapon(CurrentWeaponServer->GetWeaponType()) - 1 > 0 ?
		PFPlayerState->GetSlotOfWeapon(CurrentWeaponServer->GetWeaponType()) - 1 : 4;
	RequestSwitchWeaponWithID(LastWeaponID);
}

APFWeaponBaseServer* APFPlayerCharacter::GetWeaponServerOfType(EWeaponType WeaponType)
{
	if (WeaponListServer.Contains(WeaponType))
	{
		return WeaponListServer[WeaponType];
	}
	return nullptr;
}

APFWeaponBaseClient* APFPlayerCharacter::GetWeaponClientOfType(EWeaponType WeaponType)
{
	if (WeaponListClient.Contains(WeaponType))
	{
		return WeaponListClient[WeaponType];
	}
	return nullptr;
}

void APFPlayerCharacter::UpdateAmmoUI()
{
	if (!IsLocalPlayer) return;
	if (InGameHUD)
	{
		InGameHUD->OnMaxAmmoChanged(CurrentWeaponClient->GetMaxAmmo());
		InGameHUD->OnAmmoAmountChanged(CurrentWeaponClient->GetCurrAmmo());
	}
}


void APFPlayerCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		if (bInputReverse)
			Value = -Value;
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void APFPlayerCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		if (bInputReverse) 
			Value = -Value;

		if(AttributeComp)
			Value *= AttributeComp->GetPropertyModifier(EPropertyModifierType::PMT_Movement);
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void APFPlayerCharacter::ViewLookUp(float Value)
{
	if (Value != 0.0f)
	{
		if (bInputReverse)
			Value = -Value;
		AddControllerPitchInput(Value * MouseSensitivity);
	}
}

void APFPlayerCharacter::ViewLookRight(float Value)
{
	if (Value != 0.0f)
	{
		if (bInputReverse) 
			Value = -Value;
		AddControllerYawInput(Value * MouseSensitivity);
	}
	TurnAxisInput = Value;
}

void APFPlayerCharacter::OnRep_WeaponServer(APFWeaponBaseServer* OldValue)
{
	OnSwitchWeaponServerFinishDelegate.Broadcast(OldValue, CurrentWeaponServer);
	// Only spawn the weapon client on local player
	if (IsLocalPlayer)
	{
		SwitchWeaponClient(CurrentWeaponServer->GetWeaponType());
	}
	if (OldValue) OldValue->OnWeaponDropped();

	WeaponListServer.Add(CurrentWeaponServer->GetWeaponType(), CurrentWeaponServer);
	CurrentWeaponServer->OnWeaponEquiped(GetMesh(), WeaponServerSocket);
}

void APFPlayerCharacter::SwitchWeaponClient(EWeaponType WeaponType)
{
	EWeaponType OldWeaponType = CurrentWeaponClient ? CurrentWeaponClient->GetWeaponType() : EWeaponType::WT_None;
	if(OldWeaponType == WeaponType)
		return;

	if (CurrentWeaponClient)
		DropWeaponClient();

	FTimerHandle TimerHandle;

	FTimerDelegate TimerDelegate;

	//Binding our Lambda expression
	TimerDelegate.BindLambda([=]()
	{
		EquipWeaponClient(WeaponType);
	});

	GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, 0.3f, false);
}

void APFPlayerCharacter::DropWeaponClient()
{
	StopFire();

	CurrentWeaponClient->OnWeaponDropped();
}

void APFPlayerCharacter::EquipWeaponClient(EWeaponType NewWeaponType)
{
	APFWeaponBaseClient* NewWeapon = WeaponListClient.Contains(NewWeaponType) ? WeaponListClient[NewWeaponType] : nullptr;
	if (!NewWeapon)
	{
		NewWeapon = Cast<APFWeaponBaseClient>(APFWeaponBase::SpawnWeapon(GetWorld(), ClientWeaponClasses[NewWeaponType], GetActorTransform(), this));
		NewWeapon->SetOwner(this);
		WeaponListClient.Add(NewWeaponType, NewWeapon);
	}

	APFWeaponBase* OldWeaponClient = CurrentWeaponClient;
	EWeaponType OldWeaponType = CurrentWeaponClient ? CurrentWeaponClient->GetWeaponType() : EWeaponType::WT_None;

	CurrentWeaponClient = NewWeapon;
	CurrentWeaponClient->OnWeaponEquiped(FPMesh, WeaponClientSocket);
	CurrentWeaponClient->SyncWeaponProperties(CurrentWeaponServer);

	if (InGameHUD)
	{
		InGameHUD->OnWeaponChanged(NewWeaponType, OldWeaponType);
		UpdateAmmoUI();
	}

	OnSwitchWeaponClientFinishDelegate.Broadcast(OldWeaponClient, CurrentWeaponClient);
	SetSwitchWeaponAnim();
}

void APFPlayerCharacter::ClientFire(FVector& BulletTargetPoint, TArray<APFPlayerCharacter*>& TargetPlayers)
{
	const FRotator SpawnRotation = GetControlRotation();
	APFPlayerController* PC = Cast<APFPlayerController>(GetController());
	if (!PC) return;// OnDead

	CurrentWeaponClient->OnFireClient(PlayerCamera->GetComponentLocation(), SpawnRotation, PC, BulletTargetPoint, TargetPlayers);

	UAnimSequence* FireAnimation = CurrentWeaponClient->GetFireAnimation();
	if (FireAnimation != nullptr)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = FPMesh->GetAnimInstance();
		if (AnimInstance != nullptr)
		{
			AnimInstance->PlaySlotAnimationAsDynamicMontage(FireAnimation, TEXT("DefaultSlot"));
		}
	}
}

void APFPlayerCharacter::ServerFire_Implementation(const FVector& BulletTargetPoint, const TArray<APFPlayerCharacter*>& TargetPlayers)
{
	if (CurrentWeaponServer->GetCurrAmmo() <= 0)
	{
		APFPlayerController* PFPlayerController = Cast<APFPlayerController>(GetController());
		APFPlayerState* PFPlayerState = PFPlayerController ? PFPlayerController->GetPlayerState<APFPlayerState>() : nullptr;
		if (PFPlayerController && PFPlayerState)
		{
			EWeaponType CurrentWeaponType = CurrentWeaponServer->GetWeaponType();
			EWeaponType NewWeaponType = CurrentWeaponType;

			NewWeaponType = PFPlayerState->GetNextValidWeaponTypeWithExclusive(NewWeaponType, EWeaponType::WT_Pistol);
			//loop check if there are any weapon has ammo, if so switch to that one.
			while (WeaponListServer.Contains(NewWeaponType) && WeaponListServer[NewWeaponType]->GetCurrAmmo() <= 0)
			{
				if (NewWeaponType == CurrentWeaponType) // loop end
				{
					//if no any ammo, switch to pistol
					NewWeaponType = EWeaponType::WT_Pistol;
					break;
				}

				NewWeaponType = PFPlayerState->GetNextValidWeaponTypeWithExclusive(NewWeaponType, EWeaponType::WT_Pistol);
			}

			RequestSwitchWeaponServer(NewWeaponType);
		}
	}
	// TODO Yanyi Bao
	if (ensureMsgf(CurrentWeaponServer, TEXT("Current Weapon Server Null!")))
	{
		const FRotator SpawnRotation = GetControlRotation();
		CurrentWeaponServer->OnFireServer(PlayerCamera->GetComponentLocation(),SpawnRotation, BulletTargetPoint, TargetPlayers);
	}
}

void APFPlayerCharacter::ReLoadServer_Implementation()
{
	if (CurrentWeaponServer)
		CurrentWeaponServer->OnReLoad();
}

void APFPlayerCharacter::ClientAddAmmo_Implementation(int32 AmmoBenefits)
{
	if (CurrentWeaponClient && CurrentWeaponClient->OnAddAmmo(AmmoBenefits))
	{
		UpdateAmmoUI();
	}
}

class UPFAbilityBaseComponent* APFPlayerCharacter::GetAbilityComponent(EAbilityType AbilityType) const
{
	for (auto AbilityComp : AbilityComponents)
	{
		if (AbilityComp->GetAbilityType() == AbilityType)
		{
			return AbilityComp;
		}
	}
	return false;
}

class UPFAbilityBaseComponent* APFPlayerCharacter::GetAbilityComponentBySlotID(int32 SlotID) const
{
	if(SlotID >= 1 && SlotID <= AbilityComponents.Num())
		return AbilityComponents[SlotID - 1];
	return nullptr;
}

class USkeletalMeshComponent* APFPlayerCharacter::GetFirstPersonSkeletonMesh() const
{
	return FPMesh;
}

class UNiagaraComponent* APFPlayerCharacter::GetDamageBuffNiagara_Implementation()
{
	//Native Implementation
	return nullptr;
}

void APFPlayerCharacter::ShowScoreBoard()
{
	if (Scoreboard && Scoreboard->IsInViewport())
	{
		Scoreboard->ToggleScoreboard(false);
		return;
	}

	Scoreboard = Scoreboard ? Scoreboard : Cast<UPFScoreboardWidget>(CreateWidget(GetWorld(), ScoreboardClass));

	APFGameStateBase* GameStateBase = GetWorld() != NULL ? GetWorld()->GetGameState<APFGameStateBase>() : NULL;
	if (GameStateBase)
	{
		FPFMatchResult MatchResult = GameStateBase->GetMatchResult();

		TArray<APFPlayerState*> TeamInfo1;
		TArray<APFPlayerState*> TeamInfo2;
		const TArray<TObjectPtr<APlayerState>> playerStates = GameStateBase->PlayerArray;
		for(auto playerState: playerStates){
			APFPlayerState* state = Cast<APFPlayerState>(playerState);
			if (state->GetTeamID() == 0) {
				TeamInfo1.Add(state);
			}
			else if (state->GetTeamID() == 1) {
				TeamInfo2.Add(state);
			}
		}
		Scoreboard->InitScoreboard(MatchResult.ScoreListTeam1, MatchResult.ScoreListTeam2, MatchResult.ScoreTeam1, MatchResult.ScoreTeam2, TeamInfo1, TeamInfo2, EMatchResult::MR_Unsettled);
	}

	Scoreboard->ToggleScoreboard(true);
}

void APFPlayerCharacter::OnCharacterInitComplete()
{
	if (!IsInitComplete)
	{
		OnCharacterInitCompleteDelegate.Broadcast();
	}
	IsInitComplete = true;
}

void APFPlayerCharacter::OnDeath_Implementation()
{
	// TODO encapsulate in weapon
	SetActorHiddenInGame(true);
	if (CurrentWeaponClient)
		CurrentWeaponClient->SetActorHiddenInGame(true);
	if (CurrentWeaponServer)
		CurrentWeaponServer->SetActorHiddenInGame(true);

	if (IsFiring)
		StopFire();
}

void APFPlayerCharacter::OnRespawn_Implementation()
{
	AttributeComp->Restart();
	SetActorHiddenInGame(false);

	if (CurrentWeaponClient)
		CurrentWeaponClient->SetActorHiddenInGame(false);
	if (CurrentWeaponServer)
		CurrentWeaponServer->SetActorHiddenInGame(false);

	APFGameStateBase* GS = GetWorld() != NULL ? GetWorld()->GetGameState<APFGameStateBase>() : NULL;
	if (GS && GS->IsMatchInProgress() == true)
		EnableInput(Cast<APFPlayerController>(GetController()));

	OnCharacterRespawnDelegate.Broadcast();
}

void APFPlayerCharacter::EnterDamageImmune()
{
	int Duration = AttributeComp->GetDamageImmuneDuration();

	if (!HasAuthority() || Duration <= 0) return;

	SetCanBeDamaged(false);

	FTimerHandle TimerHandle;

	FTimerDelegate TimerDelegate;

	//Binding our Lambda expression
	TimerDelegate.BindLambda([&]()
		{
			SetCanBeDamaged(true);
		});

	GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, Duration, false);
}

void APFPlayerCharacter::RequestSwitchWeaponClient(EWeaponType WeaponType /*= EWeaponType::WT_None*/)
{
	APFPlayerController* PFPlayerController = Cast<APFPlayerController>(GetController());
	APFPlayerState* PFPlayerState = PFPlayerController ? PFPlayerController->GetPlayerState<APFPlayerState>() : nullptr;

	if (PFPlayerController && PFPlayerState)
	{
		WeaponType = WeaponType == EWeaponType::WT_None ? PFPlayerState->GetNextValidWeaponType() : WeaponType;

		if(CurrentWeaponClient && CurrentWeaponClient->GetWeaponType() == WeaponType)
			return;

		CurrentWeaponClient->OnPreDrop();
		RequestSwitchWeaponServer(WeaponType);

		StopFire();
	}
}

// Only running on server
// TODO: can not request to switch to specific weapon
void APFPlayerCharacter::RequestSwitchWeaponServer_Implementation(EWeaponType WeaponType)
{
	APFPlayerState* PFPlayerState = GetPlayerState<APFPlayerState>();

	if (PFPlayerState)
	{
		PFPlayerState->SwitchWeapon(WeaponType);

		APFWeaponBaseServer* NewWeapon = WeaponListServer.Contains(WeaponType) ? WeaponListServer[WeaponType] : nullptr;
		if (!NewWeapon)
		{
			NewWeapon = Cast<APFWeaponBaseServer>(APFWeaponBase::SpawnWeapon(GetWorld(), ServerWeaponClasses[WeaponType], GetActorTransform(), this));
			NewWeapon->SetOwner(this);
			WeaponListServer.Add(WeaponType, NewWeapon);
		}

		if (CurrentWeaponServer && CurrentWeaponServer->GetWeaponType() != WeaponType)
			CurrentWeaponServer->OnWeaponDropped();

		CurrentWeaponServer = NewWeapon;
		// Reliable Broadcast to all clients to equip third person weapon
		CurrentWeaponServer->OnWeaponEquiped(GetMesh(), WeaponServerSocket);
	}
}

bool APFPlayerCharacter::RequestSwitchWeaponServer_Validate(EWeaponType WeaponType)
{
	return true;
}

void APFPlayerCharacter::RequestSwitchWeaponWithID(int32 SlotID)
{
	APFPlayerState* PFPlayerState = GetPlayerState<APFPlayerState>();

	if (PFPlayerState)
	{
		EWeaponType WeaponType = PFPlayerState->GetWeaponTypeOfSlot(SlotID);

		RequestSwitchWeaponClient(WeaponType);
	}
}

float APFPlayerCharacter::GetGravityZ() const
{
	return GetMovementComponent()->GetGravityZ();
}

void APFPlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APFPlayerCharacter, CurrentWeaponServer);
	DOREPLIFETIME_CONDITION(APFPlayerCharacter, AbilityComponents, COND_OwnerOnly);
}

//UNiagaraComponent* APFPlayerCharacter::GetDamageBuffNiagara()
//{
//	//Native implementation
//	return nullptr;
//}

TArray<UMeshComponent*> APFPlayerCharacter::GetAllThirdPersonMeshes()
{
	TArray<UMeshComponent*> meshes;
	meshes.Add(GetMesh());
	//meshes.Add(GetCurrentWeaponServer()->GetMesh());
	TArray<UMeshComponent*> meshesFromBP = GetThirdPersonMeshesFromBP();
	if (meshesFromBP.Num() != 0) meshes.Append(meshesFromBP);
	return meshes;
}

TArray<UMeshComponent*> APFPlayerCharacter::GetAllFirstPersonMeshes()
{
	TArray<UMeshComponent*> meshes;
	meshes.Add(FPMesh);
	//meshes.Add(GetCurrentWeaponClient()->GetMesh());
	TArray<UMeshComponent*> meshesFromBP = GetFirstPersonMeshesFromBP();
	if (meshesFromBP.Num() != 0) meshes.Append(meshesFromBP);
	return meshes;
}
