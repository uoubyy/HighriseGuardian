// Fill out your copyright notice in the Description page of Project Settings.


#include "PFAttributeComponent.h"
#include "Net/UnrealNetwork.h"
#include "../PFGameModeBase.h"
#include "Perk/PFWeaponModifierInterface.h"
#include "PFPerkBaseComponent.h"
#include "Perk/PFMovementModifierInterface.h"
#include "Perk/PFHealthModifierInterface.h"

UPFAttributeComponent* UPFAttributeComponent::GetAttributes(AActor* FromActor)
{
	if (FromActor)
	{
		return FromActor->FindComponentByClass<UPFAttributeComponent>();
	}

	return nullptr;
}

bool UPFAttributeComponent::IsActorAlive(AActor* Actor)
{
	UPFAttributeComponent* AttributeComp = GetAttributes(Actor);
	if (AttributeComp)
	{
		return AttributeComp->IsAlive();
	}

	return false;
}

UPFAttributeComponent::UPFAttributeComponent()
{
	HealthMax = 100;
	Health = HealthMax;

	PropertyModifierList.Add(EPropertyModifierType::PMT_Movement, 1.0f);
	PropertyModifierList.Add(EPropertyModifierType::PMT_Damage, 1.0f);
	PropertyModifierList.Add(EPropertyModifierType::PMT_Health, 0.0f);

	SetIsReplicatedByDefault(true);

	PrimaryComponentTick.bCanEverTick = true;
}

void UPFAttributeComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdatepPropertyModifier(EPropertyModifierType::PMT_Movement);
	UpdatepPropertyModifier(EPropertyModifierType::PMT_Damage);
	UpdatepPropertyModifier(EPropertyModifierType::PMT_Health);
}

void UPFAttributeComponent::OnInvisiblityChange_Implementation()
{
	//Native implementation
}

void UPFAttributeComponent::MulticastHealthChanged_Implementation(AActor* InstigatorActor, float NewHealth, float Delta)
{
	OnHealthChangedDelegate.Broadcast(InstigatorActor, this, NewHealth, Delta);
	//UE_LOG(LogTemp, Warning, TEXT("Health Changed %f %f"), NewHealth, Delta);
}

void UPFAttributeComponent::MulticastCauseDamage_Implementation(AActor* VictimActor, float NewHealth, float Delta)
{
	OnCauseDamageDelegate.Broadcast(VictimActor, NewHealth, Delta);

	if (GetOwner()->HasAuthority() && FMath::IsNearlyEqual(NewHealth, 0.0f))
	{
		float HealthModifier = GetPropertyModifier(EPropertyModifierType::PMT_Health);

		float Temp = FMath::Clamp(Health + HealthMax * HealthModifier, 0, HealthMax);
		Health = Temp;

		MulticastHealthChanged(nullptr, Health, Temp - Health);
	}
}

void UPFAttributeComponent::Restart()
{
	Health = HealthMax;
	MulticastHealthChanged(nullptr, Health, 0);
}

bool UPFAttributeComponent::IsAlive() const
{
	return Health > 0.0f;
}


bool UPFAttributeComponent::IsFullHealth() const
{
	return Health == HealthMax;
}


float UPFAttributeComponent::GetHealth() const
{
	return Health;
}

float UPFAttributeComponent::GetHealthMax() const
{
	return HealthMax;
}

float UPFAttributeComponent::GetDamageImmuneDuration() const
{
	return DamageImmuneDuration;
}

bool UPFAttributeComponent::GetVisiblity() const
{
	return Invisiblity;
}

bool UPFAttributeComponent::ApplyHealthChange(AActor* InstigatorActor, float Delta)
{
	// InstigatorActor: Damage Causer of Heal Causer
	if (!GetOwner()->CanBeDamaged() && Delta < 0.0f)
	{
		return false;
	}

	float OldHealth = Health;
	float NewHealth = FMath::Clamp(Health + Delta, 0.0f, HealthMax);

	float ActualDelta = NewHealth - OldHealth;

	// Is Server?
	if (GetOwner()->HasAuthority())
	{
		Health = NewHealth;

		if (ActualDelta != 0.0f)
		{
			MulticastHealthChanged(InstigatorActor, Health, ActualDelta);
		}

		if (ActualDelta < 0.0f && InstigatorActor != GetOwner())
		{
			UPFAttributeComponent* CauserComp = UPFAttributeComponent::GetAttributes(InstigatorActor);
			if (CauserComp)
			{
				CauserComp->MulticastCauseDamage(GetOwner(), Health, ActualDelta);
			}
		}

		// Died
		if (ActualDelta < 0.0f && FMath::IsNearlyEqual(Health, 0.0f))
		{
			APFGameModeBase* GM = GetWorld()->GetAuthGameMode<APFGameModeBase>();
			if (GM)
			{
				GM->OnActorKilled(GetOwner(), InstigatorActor);
			}
		}
	}

	return ActualDelta != 0;
}

bool UPFAttributeComponent::ApplyInvisiblityChange(bool Invisible)
{
	// Is Server?
	if (GetOwner()->HasAuthority())
	{
		Invisiblity = Invisible;
		return true;
	}
	return false;
}

bool UPFAttributeComponent::SetPropertyModifier(EPropertyModifierType ModifierType, float ModifierValue, TArray<FPropertyModifierDelta>& PropertyModifierUpdateList)
{
	float& Value = PropertyModifierList.FindOrAdd(ModifierType);
	if (!FMath::IsNearlyZero(Value - ModifierValue))
	{
		FPropertyModifierDelta PropertyModifierDelta;
		PropertyModifierDelta.PropertyModifierType = ModifierType;
		PropertyModifierDelta.PropertyModifierOldValue = Value;
		PropertyModifierDelta.PropertyModifierNewValue = ModifierValue;

		PropertyModifierUpdateList.Add(PropertyModifierDelta);

		FString ModifierTypeName = GetModifierTypeName(ModifierType);
		UE_LOG(LogTemp, Warning, TEXT("SetPropertyModifier %s %f %f"), *ModifierTypeName, Value, ModifierValue);
		Value = ModifierValue;
		return true;
	}
	return false;
}

// Sync the Property Modifier from Server and Broadcast
void UPFAttributeComponent::OnPropertyModifierChanged_Implementation(const TArray<FPropertyModifierDelta>& PropertyModifierUpdateList)
{
	for (const auto& ModifierDelta : PropertyModifierUpdateList)
	{
		float& Value = PropertyModifierList.FindOrAdd(ModifierDelta.PropertyModifierType);
		Value = ModifierDelta.PropertyModifierNewValue;
	}

	OnPropertyModifierChangedDelegate.Broadcast(PropertyModifierUpdateList);
	UE_LOG(LogTemp, Warning, TEXT("Broadcast PropertyModifierChanged Client"));
}

void UPFAttributeComponent::UpdatepPropertyModifier(EPropertyModifierType UpdateTarget)
{
	float DamageModifier = 0.0;
	float MovementModifier = 0.0;
	float HealthModifier = 0.0;

	bool DamageModifierChanged = false, MovementModifierChanged = false, HealthModifierChanged = false;
	TArray<FPropertyModifierDelta> PropertyModifierUpdateList;

	for (auto Perk : ActivePerkList)
	{
		if (Perk->IsPerkActive())// && Perk->IsDirty())
		{
			if (UpdateTarget == EPropertyModifierType::PMT_Movement && 
				Perk->GetClass()->ImplementsInterface(UPFMovementModifierInterface::StaticClass()))
			{
				IPFMovementModifierInterface* MovementModifierInterface = Cast<IPFMovementModifierInterface>(Perk);
				MovementModifier += MovementModifierInterface->Execute_S_HorizentalMoveSpeedModify(Perk);

				MovementModifierChanged = true;
			}
			
			if (UpdateTarget == EPropertyModifierType::PMT_Damage && 
				Perk->GetClass()->ImplementsInterface(UPFWeaponModifierInterface::StaticClass()))
			{
				IPFWeaponModifierInterface* WeaponModifierInterface = Cast<IPFWeaponModifierInterface>(Perk);
				DamageModifier += WeaponModifierInterface->Execute_S_DamageModify(Perk);

				DamageModifierChanged = true;
			}
			
			if (UpdateTarget == EPropertyModifierType::PMT_Health && 
				Perk->GetClass()->ImplementsInterface(UPFHealthModifierInterface::StaticClass()))
			{
				IPFHealthModifierInterface* HealthModifierInterface = Cast<IPFHealthModifierInterface>(Perk);
				HealthModifier += HealthModifierInterface->Execute_S_HealthRatioChangeOnKillEnemy(Perk);

				HealthModifierChanged = true;
			}
		}
	}

	if(MovementModifierChanged)
		SetPropertyModifier(EPropertyModifierType::PMT_Movement, 1.0 + MovementModifier, PropertyModifierUpdateList);
	if(DamageModifierChanged)
		SetPropertyModifier(EPropertyModifierType::PMT_Damage, 1.0 + DamageModifier, PropertyModifierUpdateList);
	if(HealthModifierChanged)
		SetPropertyModifier(EPropertyModifierType::PMT_Health, HealthModifier, PropertyModifierUpdateList);

	if(PropertyModifierUpdateList.Num() > 0)
	{ 
		OnPropertyModifierChangedDelegate.Broadcast(PropertyModifierUpdateList);
		OnPropertyModifierChanged(PropertyModifierUpdateList);
		UE_LOG(LogTemp, Warning, TEXT("Broadcast PropertyModifierChanged Server, Count %d, ActivePerkList Count %d."), PropertyModifierUpdateList.Num(), ActivePerkList.Num());
	}
}

void UPFAttributeComponent::RegisterPerk(class UPFPerkBaseComponent* PerkComp)
{
	ActivePerkList.AddUnique(PerkComp);

	UE_LOG(LogTemp, Warning, TEXT("RegisterPerk %s."), *PerkComp->GetPerkName());
}

bool UPFAttributeComponent::UnRegisterPerk(class UPFPerkBaseComponent* PerkComp)
{
	if(!ActivePerkList.Contains(PerkComp))
		return false;

	UE_LOG(LogTemp, Warning, TEXT("UnRegisterPerk %s."), *PerkComp->GetPerkName());

	ActivePerkList.Remove(PerkComp);
	return true;
}

void UPFAttributeComponent::RegisterPerkByType(EPerkType PerkType)
{
	
}

bool UPFAttributeComponent::UnRegisterPerkByType(EPerkType PerkType)
{
	return true;
}

float UPFAttributeComponent::GetPropertyModifier(EPropertyModifierType TargetProperty)
{
	if (!PropertyModifierList.Contains(TargetProperty))
		PropertyModifierList.Add(TargetProperty, 1.0f);

	return PropertyModifierList[TargetProperty];
}

void UPFAttributeComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UPFAttributeComponent, Health);
	DOREPLIFETIME_CONDITION(UPFAttributeComponent, HealthMax, COND_OwnerOnly);

	DOREPLIFETIME_CONDITION(UPFAttributeComponent, DamageImmuneDuration, COND_OwnerOnly);
	DOREPLIFETIME(UPFAttributeComponent, Invisiblity);
}