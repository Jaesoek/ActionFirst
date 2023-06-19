// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ActionFirst.h"
#include "GameFramework/HUD.h"
#include "InGameHUD.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONFIRST_API AInGameHUD : public AHUD
{
	GENERATED_BODY()

public:
	AInGameHUD();

	virtual void BeginPlay() override;

	// TODO: CoolDown process run
	void CoolDownDash();

public:
	UPROPERTY(EditDefaultsOnly, Category = "Interactive")
	TSubclassOf<UUserWidget> HUD_WidgetClass;

private:
	UUserWidget* HUD_Widget;
};
