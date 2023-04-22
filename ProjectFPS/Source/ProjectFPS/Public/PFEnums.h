#pragma once
#include "CoreMinimal.h"
#include "PFEnums.generated.h"

UENUM(BlueprintType)
enum class EMatchMode : uint8
{
	MM_None	UMETA(DisplayName = "None"),
	MM_1V1	UMETA(DisplayName = "1 vs 1"),
	MM_3V3	UMETA(DisplayName = "3 vs 3"),
	Count	UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EMatchResult : uint8
{
	MR_Unsettled	UMETA(DisplayName = "Unsettled"),
	MR_Won			UMETA(DisplayName = "Won"),
	MR_Lose			UMETA(DisplayName = "Lose"),
	MR_Draw			UMETA(DisplayName = "Draw"),
	Count			UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EAbilityInputID : uint8
{
	// 0 None
	None			UMETA(DisplayName = "None"),
	Confirm			UMETA(DisplayName = "Confirm"),
	Cancel			UMETA(DisplayName = "Cancel"),

	Jump			UMETA(DisplayName = "Jump"),

	Ability1		UMETA(DisplayName = "Ability1"),
	Ability2		UMETA(DisplayName = "Ability2"),
	Ability3		UMETA(DisplayName = "Ability3"),
	Ability4		UMETA(DisplayName = "Ability4"),
	Ability5		UMETA(DisplayName = "Ability5"),
};

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	WT_None				UMETA(DisplayName = "None"),
	WT_SemiAuto			UMETA(DisplayName = "Semi-Auto"),
	WT_ShotGun			UMETA(DisplayName = "ShotGun"),
	WT_AutoRifle		UMETA(DisplayName = "AutoRifle"),
	WT_RocketLauncher	UMETA(DisplayName = "RocketLauncher"),
	WT_Pistol			UMETA(DisplayName = "Pistol"),
	Count				UMETA(Hidden)
};

ENUM_RANGE_BY_COUNT(EWeaponType, EWeaponType::Count);

EWeaponType GetNextWeaponType(EWeaponType Current);
EWeaponType GetWeaponTypeByID(int32 ID);


UENUM(BlueprintType)
enum class EWeaponStatusType : uint8
{
	WST_NotEquipped			UMETA(DisplayName = "Not Equipped"),
	WST_PreEquip			UMETA(DisplayName = "Pre Equip"),
	WST_Equipped			UMETA(DisplayName = "Equipped"),
	WST_PreDrop				UMETA(DisplayName = "Pre Drop"),
	Count					UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EAbilityType : uint8
{
	AT_None			UMETA(DisplayName = "AbilityNone"),
	AT_Dash			UMETA(DisplayName = "AbilityDash"),
	AT_Invisible	UMETA(DisplayName = "AbilityInvisible"),
	AT_Vision		UMETA(DisplayName = "AbilityVision"),
	AT_Shock		UMETA(DisplayName = "AbilityShock"),
	Count			UMETA(Hidden)
};

ENUM_RANGE_BY_COUNT(EAbilityType, EAbilityType::Count);

EAbilityType GetNextAbilityType(EAbilityType Current);

UENUM(BlueprintType)
enum class EPowerupType : uint8
{
	PT_AmmoBox		UMETA(DisplayName = "Ammo Box"),
	PT_HealthBox	UMETA(DisplayName = "Health Box"),
	PT_None			UMETA(DisplayName = "None"),
	Count			UMETA(Hidden)
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPopupConfirm);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPopupCancel);

USTRUCT(BlueprintType)
struct FInGameError
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GamePlay|Error")
	int32 Priority;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GamePlay|Error")
	int32 ErrorCode;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GamePlay|Error")
	FString Message;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GamePlay|Error")
	FOnPopupConfirm OnPopupConfirmDelegate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GamePlay|Error")
	FOnPopupCancel OnPopupCancelDelegate;

	FInGameError()
	{
		Priority = 0;
		ErrorCode = 100;
		Message = FString("In Game Error.");
	}

	FInGameError(int32 _Priority, int32 _ErrorCode, FString _Message)
	{
		Priority = _Priority;
		ErrorCode = _ErrorCode;
		Message = _Message;
	}
};

USTRUCT(BlueprintType)
struct FPFScoreInfo
{
	GENERATED_USTRUCT_BODY();

	UPROPERTY(BlueprintReadOnly)
	FString PlayerName;

	UPROPERTY(BlueprintReadOnly)
	int KilledNum;

	UPROPERTY(BlueprintReadOnly)
	int DeadNum;

	FPFScoreInfo() 
	{
		PlayerName = "";
		KilledNum = 0;
		DeadNum = 0;
	}

	FPFScoreInfo(const FString& Name, const int Killed, const int Dead)
	{
		PlayerName = Name;
		KilledNum = Killed;
		DeadNum = Dead;
	}
};

USTRUCT(BlueprintType)
struct FPFMatchResult
{
	GENERATED_USTRUCT_BODY();

	UPROPERTY(BlueprintReadOnly)
	TArray<FPFScoreInfo> ScoreListTeam1;

	UPROPERTY(BlueprintReadOnly)
	TArray<FPFScoreInfo> ScoreListTeam2;

	UPROPERTY(BlueprintReadOnly)
	int32 ScoreTeam1;

	UPROPERTY(BlueprintReadOnly)
	int32 ScoreTeam2;

	FPFMatchResult()
	{
		ScoreTeam1 = 0;
		ScoreTeam2 = 0;
	}
};

USTRUCT(BlueprintType)
struct FKillRecord
{
	GENERATED_USTRUCT_BODY();

	UPROPERTY(BlueprintReadOnly)
	FString CauserName;

	UPROPERTY(BlueprintReadOnly)
	EWeaponType WeaponType;

	UPROPERTY(BlueprintReadOnly)
	FString VictimName;

	FKillRecord()
	{
		CauserName = "";
		WeaponType = EWeaponType::WT_None;
		VictimName = "";
	}
};

UENUM(BlueprintType)
enum class EPerkType : uint8
{
	PT_None			UMETA(DisplayName = "PerkNone"),
	PT_FOF      	UMETA(DisplayName = "Flight or Flight"),
	PT_Stress	    UMETA(DisplayName = "Streess Reaction"),
	PT_Vampire		UMETA(DisplayName = "Vampire"),
	PT_Concentrate	UMETA(DisplayName = "Concentrate"),
	Count           UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EPropertyModifierType : uint8
{
	PMT_None			UMETA(DisplayName = "None"),
	PMT_Movement		UMETA(DisplayName = "Movement"),
	PMT_Damage			UMETA(DisplayName = "Damage"),
	PMT_Health			UMETA(DisplayName = "Health"),
	PMT_All				UMETA(DisplayName = "All"),
	Count
};

ENUM_RANGE_BY_COUNT(EPerkType, EPerkType::Count);

EPerkType GetNextPerkType(EPerkType Current);

USTRUCT(BlueprintType)
struct FPropertyModifierDelta
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Property Modifier")
	EPropertyModifierType PropertyModifierType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Property Modifier")
	float PropertyModifierNewValue;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Property Modifier")
	float PropertyModifierOldValue;

	FPropertyModifierDelta()
	{
		PropertyModifierType = EPropertyModifierType::PMT_None;
		PropertyModifierNewValue = 0.0f;
		PropertyModifierOldValue = 0.0f;
	}
};

FString GetModifierTypeName(EPropertyModifierType ModifierType);