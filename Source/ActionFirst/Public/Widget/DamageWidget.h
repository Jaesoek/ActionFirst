// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ActionFirst.h"
#include "Blueprint/UserWidget.h"
#include "DamageWidget.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONFIRST_API UDamageWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void OnAnimationFinished_Implementation(const UWidgetAnimation* Animation) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* UI_Ani_Damage;

public:
	void setPlayer(AController* player) { OwnPlayerController = player; };
	void setAmountDamage(float damage) { AmountDamage = damage; };

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (ExposeOnSpawn = true, AllowPrivateAccess = true))
	AController* OwnPlayerController;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ExposeOnSpawn = true, AllowPrivateAccess = true))
	float AmountDamage;
};
