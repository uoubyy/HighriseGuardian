// Copyright FPS Team. All Rights Reserved.


#include "UI/PFPickupIndicatorWidget.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Border.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"
#include "PFPickUpBase.h"
#include "DataTable/PFDataTableRowWeapon.h"

void UPFPickupIndicatorWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (!PlayerController) return;

	static int32 ScreenWidth = 0, ScreenHeight = 0;
	if (ScreenWidth == 0 || ScreenHeight == 0)
	{
		PlayerController->GetViewportSize(ScreenWidth, ScreenHeight);
		APlayerCameraManager* CameraManager = PlayerController->PlayerCameraManager;
	}

	ACharacter* PlayerCharacter = PlayerController ? static_cast<ACharacter*>(PlayerController->GetPawn()) : nullptr;

	if (PlayerCharacter && IsActive && PickupActor.IsValid())
	{
		FVector2D ScreenLocation(0.0);
		FVector2D HalfScreen = FVector2D(ScreenWidth / 2.0, ScreenHeight / 2.0f);
		FVector2D ScreenBorder(ScreenWidth* 0.1, ScreenHeight * 0.12);

		FRotator RelevantRot = UKismetMathLibrary::FindLookAtRotation(PlayerCharacter->GetActorLocation(), PickupActor->GetActorLocation());
		FRotator CharacterRot = PlayerCharacter->GetActorRotation();
		float Angle = (RelevantRot - CharacterRot).Yaw;
		float AngleInRadians = FMath::DegreesToRadians(Angle);

		bool TryProjection = PlayerController->ProjectWorldLocationToScreen(PickupActor->GetActorLocation(), ScreenLocation);

		if (TryProjection)
		{
			ScreenLocation -= HalfScreen;
		}

		if(TryProjection == false)
		{
			if (Angle > -90.0f && Angle < 90.0f)
			{
				float IntersectionX = 2 * HalfScreen.Y * FMath::Tan(AngleInRadians);
				float LeftOrRight = Angle < 0.0f ? -1.0 : 1.0f;

				if (IntersectionX >= -HalfScreen.X && IntersectionX <= HalfScreen.X)
				{
					ScreenLocation = FVector2D(IntersectionX, -HalfScreen.Y);
				}
				else
				{
					float IntersectionY = HalfScreen.X * FMath::Tan(FMath::DegreesToRadians(90.0f - FMath::Abs(Angle)));
					ScreenLocation = FVector2D(LeftOrRight * HalfScreen.X, HalfScreen.Y - IntersectionY);
				}
			}
			else
			{
				float LeftOrRight = Angle < -90.0f ? -1.0 : 1.0f;
				float IntersectionX = HalfScreen.X * FMath::Tan(FMath::DegreesToRadians(180.0 - FMath::Abs(Angle)));
				ScreenLocation = FVector2D(LeftOrRight * IntersectionX, HalfScreen.Y);
			}
		}

		float ScreenLocationX = FMath::Clamp(ScreenLocation.X, -HalfScreen.X + ScreenBorder.X, HalfScreen.X - ScreenBorder.X);
		float ScreenLocationY = FMath::Clamp(ScreenLocation.Y, -HalfScreen.Y + ScreenBorder.Y, HalfScreen.Y - ScreenBorder.Y);

		ScreenLocation = FVector2D(ScreenLocationX, ScreenLocationY);

		UCanvasPanelSlot* CanvasPanelSlot = Cast<UCanvasPanelSlot>(this->Slot);
		if (CanvasPanelSlot)
		{
			CanvasPanelSlot->SetSize(FVector2D(100));
			CanvasPanelSlot->SetPosition(ScreenLocation);
		}
	}
}

void UPFPickupIndicatorWidget::BeginDestroy()
{
	Super::BeginDestroy();
}

void UPFPickupIndicatorWidget::InitPickupIndicator(AActor* TargetActor, EPowerupType TargetType)
{
	PickupActor = TargetActor;
	PickupType = TargetType;

	if ( TargetType == EPowerupType::PT_AmmoBox )
	{
		FString ContextString;
		APFPickUpBase* PickUpActor = Cast<APFPickUpBase>(TargetActor);
		if(PickUpActor)
		{
			FName RowName = UEnum::GetValueAsName(PickUpActor ->GetTargetWeaponType());
			FPFDataTableRowWeapon* DataRow = WeaponsInfoDataTable->FindRow<FPFDataTableRowWeapon>(RowName, ContextString);
			if (DataRow)
			{
				PickupIndicator->SetBrushFromTexture(DataRow->IndicatorIcon);
			}
		}
	}
	else
	{
		PickupIndicator->SetBrushFromTexture(HealthBoxIcon);
	}
}

EPowerupType UPFPickupIndicatorWidget::GetPickupIndicatorTargetType()
{
	return PickupType;
}

EWeaponType UPFPickupIndicatorWidget::GetPickupIndicatorTargetWeaponType()
{
	if (PickupType == EPowerupType::PT_AmmoBox)
	{
		APFPickUpBase* PickUpActor = Cast<APFPickUpBase>(PickupActor);
		if (PickUpActor)
			return PickUpActor->GetTargetWeaponType();
	}
	return EWeaponType::WT_None;
}

void UPFPickupIndicatorWidget::TogglePickupIndicator(bool Value)
{
	IsActive = Value;
	UCanvasPanelSlot* CanvasPanelSlot = Cast<UCanvasPanelSlot>(this->Slot);
	if (Value && CanvasPanelSlot)
	{
		CanvasPanelSlot->SetSize(FVector2D(100));
		CanvasPanelSlot->SetAlignment(FVector2D(0.5));
		CanvasPanelSlot->SetAnchors(FAnchors(0.5));
		SetVisibility(ESlateVisibility::HitTestInvisible);
	}
	else
	{
		SetVisibility(ESlateVisibility::Hidden);
	}
}
