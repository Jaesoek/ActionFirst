// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/DamageWidget.h"

void UDamageWidget::OnAnimationFinished_Implementation(const UWidgetAnimation* Animation)
{
	Super::OnAnimationFinished_Implementation(Animation);
	RemoveFromParent();
}