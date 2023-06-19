// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ActionFirst.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_Rotation.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONFIRST_API UBTTask_Rotation : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_Rotation();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* nodeMemory) override;
};
