// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ActionFirst.h"
#include "GameFramework/GameModeBase.h"
#include "ActionGameMode.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONFIRST_API AActionGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AActionGameMode();

protected:
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void StartPlay() override;
};
