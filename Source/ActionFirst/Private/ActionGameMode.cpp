// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionGameMode.h"
#include "UserCharacter.h"
#include "MainPlayerController.h"

AActionGameMode::AActionGameMode()
{
	ABLOG(Warning, TEXT("AActionGameMode"));

	DefaultPawnClass = AUserCharacter::StaticClass();
	PlayerControllerClass = AMainPlayerController::StaticClass();
}

void AActionGameMode::PostLogin(APlayerController* NewPlayer)
{
	ABLOG(Warning, TEXT("AActionGameMode PostLogin Begin"));
	Super::PostLogin(NewPlayer);
	ABLOG(Warning, TEXT("AActionGameMode PostLogin End"));
}

void AActionGameMode::StartPlay()
{
	ABLOG(Warning, TEXT("AActionGameMode StartPlay Begin"));
	Super::StartPlay();
	ABLOG(Warning, TEXT("AActionGameMode StartPlay End"));
}
