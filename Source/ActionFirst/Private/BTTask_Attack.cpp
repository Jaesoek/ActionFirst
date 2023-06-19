// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_Attack.h"
#include "EnemyAIController.h"
#include "EnemyCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_Attack::UBTTask_Attack()
{
//	bNotifyTick = true;
	NodeName = TEXT("Attack");
}

EBTNodeResult::Type UBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* nodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, nodeMemory);

	auto ControllingPawn = Cast<AEnemyCharacter>(OwnerComp.GetAIOwner()->GetPawn());
	if (nullptr == ControllingPawn) return EBTNodeResult::Failed;

	ControllingPawn->Attack();
	ControllingPawn->OnAttackEnd.AddLambda(
		/* �̰Ŵ� �Ǵ°� */
		[&]() -> void {
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		}
		/* �̰Ŵ� �ȵǴ� ��
		[OwnerComp]() -> void {
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		}
		*/
	);

	return EBTNodeResult::InProgress;
}

void UBTTask_Attack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	auto ControllingPawn = Cast<AEnemyCharacter>(OwnerComp.GetAIOwner()->GetPawn());
	if (nullptr == ControllingPawn) FinishLatentTask(OwnerComp, EBTNodeResult::Failed);

	if (!(ControllingPawn->isAttacking))
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}