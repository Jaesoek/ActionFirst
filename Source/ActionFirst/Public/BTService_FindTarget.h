// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ActionFirst.h"
#include "BehaviorTree/BTService.h"
#include "BTService_FindTarget.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONFIRST_API UBTService_FindTarget : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_FindTarget();

	UPROPERTY(VisibleAnywhere)
	float timerSum = 0.f;

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
	bool isPlayedAnimation = false;

	const float MAX_WAIT_TIMER = 3.f;

	/*
	inline float CustomClamp(const float delta) {
		if (timerSum + delta > MAX_WAIT_TIMER) timerSum = 0;
		else timerSum += delta;
	};
	*/
};
