// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_Rotation.h"
#include "EnemyAIController.h"
#include "EnemyCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"


UBTTask_Rotation::UBTTask_Rotation()
{
	NodeName = TEXT("Rotate_to_target");
}

EBTNodeResult::Type UBTTask_Rotation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* nodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, nodeMemory);

	auto ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (nullptr == ControllingPawn) return EBTNodeResult::Failed;

	auto Target = Cast<APawn>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AEnemyAIController::TargetKey));
	if (nullptr == Target) return EBTNodeResult::Failed;

	auto lookVector = Target->GetActorLocation() - ControllingPawn->GetActorLocation();
	FRotator TargetRot = FRotationMatrix::MakeFromX(lookVector).Rotator();
	ControllingPawn->SetActorRotation(
		FMath::RInterpTo(
			ControllingPawn->GetActorRotation(), TargetRot, GetWorld()->GetDeltaSeconds(), 2.f)
	);
	return EBTNodeResult::Succeeded;
}