// Copyright FPS Team. All Rights Reserved.


#include "UI/PFHitIndicatorWidget.h"
//#include "Kismet/KismetMathLibrary.h"

//void UPFHitIndicatorWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
//{
//	Super::NativeTick(MyGeometry, InDeltaTime);
//
//	if(IsActive && CauserActor && VictimActor)
//	{
//		LifeTime -= InDeltaTime;
//
//		if(LifeTime <= 0.0f)
//		{ 
//			FadeOut();
//			IsActive = false;
//		}
//
//		FRotator RelevantRot = UKismetMathLibrary::FindLookAtRotation(VictimActor->GetActorLocation(), CauserActor->GetActorLocation());
//		FRotator VictimRot = VictimActor->GetActorRotation();
//		SetRenderTransformAngle((RelevantRot - VictimRot).Yaw);
//	}
//}

void UPFHitIndicatorWidget::ActiveHitIndicator(FRotator DamageDirection)
{
	//if (Causer == Victim) return;

	//CauserActor = Causer;
	//VictimActor = Victim;

	//LifeTime = MaxVisibleTime;

	if(!IsActive)
		FadeIn();

	SetRenderTransformAngle(DamageDirection.Yaw);

	IsActive = true;

	GetWorld()->GetTimerManager().SetTimer(LifeTimerHandler, this, &UPFHitIndicatorWidget::DeActiveHitIndicator, MaxVisibleTime, false);
}

void UPFHitIndicatorWidget::DeActiveHitIndicator()
{
	FadeOut();
	IsActive = false;
}
