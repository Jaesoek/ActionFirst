// Fill out your copyright notice in the Description page of Project Settings.


#include "MainPlayerController.h"
#include "Blueprint/UserWidget.h"


AMainPlayerController::AMainPlayerController()
{
	// Add HUD
	static ConstructorHelpers::FClassFinder<UUserWidget> USER_WIDGET(
		TEXT("/Game/OwnAssets/UI/UHD_InGame.UHD_InGame_C")
	);
	if (USER_WIDGET.Succeeded())
	{
		HUD_WidgetClass = USER_WIDGET.Class;
	}

	ABLOG(Warning, TEXT("AMainPlayerController"));
}

void AMainPlayerController::BeginPlay()
{
	ABLOG(Warning, TEXT("AMainPlayerController BeginPlay Begin"));
	Super::BeginPlay();
	ABLOG(Warning, TEXT("AMainPlayerController BeginPlay End"));

	if (HUD_WidgetClass != nullptr)
	{
		HUD_Widget = CreateWidget<UUserWidget>(GetWorld(), HUD_WidgetClass);
		if (HUD_Widget != nullptr)
		{
			HUD_Widget->AddToViewport();
		}
	}
	else
	{
		ABLOG(Warning, TEXT("TEST USER_WIDGET failed"));
	}
}

void AMainPlayerController::OnPossess(APawn* aPawn)
{
	ABLOG(Warning, TEXT("AMainPlayerController OnPossess Begin"));
	Super::OnPossess(aPawn);
	ABLOG(Warning, TEXT("AMainPlayerController OnPossess End"));
}