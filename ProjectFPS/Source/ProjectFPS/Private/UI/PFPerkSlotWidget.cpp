// Copyright FPS Team. All Rights Reserved.


#include "UI/PFPerkSlotWidget.h"
#include "Components/Image.h"
#include "Components/Button.h"

#include "Styling/SlateBrush.h"
#include "Styling/SlateStyle.h"

void UPFPerkSlotWidget::InitSlot(EPerkType ItemType, class UTexture2D* Icon)
{
	PerkType = ItemType;
	SlotButton->WidgetStyle.Normal.SetResourceObject(Icon);
	SlotButton->WidgetStyle.Normal.OutlineSettings.Width = 0.0;
	SlotButton->WidgetStyle.Hovered.SetResourceObject(Icon);
	SlotButton->WidgetStyle.Hovered.OutlineSettings.Width = 0.0;
	SlotButton->WidgetStyle.Pressed.SetResourceObject(Icon);
	SlotButton->WidgetStyle.Pressed.OutlineSettings.Width = 0.0;
}
