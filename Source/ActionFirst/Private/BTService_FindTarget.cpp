// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_FindTarget.h"
#include "EnemyAIController.h"
#include "UserCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "DrawDebugHelpers.h"
#include "EnemyCharacter.h"
#include "Components/WidgetComponent.h"

UBTService_FindTarget::UBTService_FindTarget()
{
	NodeName = TEXT("FindTarget");
}

static UWidgetAnimation* GetAnimation(UProperty* prop, void* PropertyAddress);

void UBTService_FindTarget::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (nullptr == ControllingPawn) return;

	UWorld* World = ControllingPawn->GetWorld();
	if (nullptr == World) return;

	FVector Center = ControllingPawn->GetActorLocation();
	float DetectRadius = 600.f;

	TArray<FOverlapResult> OverlapResults;
	
	FCollisionQueryParams IgnoreOwner = FCollisionQueryParams::DefaultQueryParam;
	IgnoreOwner.AddIgnoredActor(ControllingPawn);

	bool bResult = World->OverlapMultiByChannel(
		OverlapResults,
		Center,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel2,
		FCollisionShape::MakeSphere(DetectRadius),
		IgnoreOwner
	);

	if (bResult)
	{
		for (auto const& OverlapResult : OverlapResults)
		{
			AUserCharacter* UserChar = Cast<AUserCharacter>(OverlapResult.GetActor());
			if (UserChar && UserChar->GetController()->IsPlayerController())
			{
				OwnerComp.GetBlackboardComponent()->SetValueAsObject(AEnemyAIController::TargetKey, UserChar);

				auto enemy = Cast<AEnemyCharacter>(ControllingPawn);
				if (enemy != nullptr && !isPlayedAnimation)
				{
					isPlayedAnimation = true;

					auto widgetObject = enemy->NotifyIcon->GetUserWidgetObject();
					auto prop = widgetObject->GetClass()->PropertyLink;
					widgetObject->PlayAnimation(GetAnimation(prop, widgetObject));
				}
			}
		}
	}
	else
	{
		isPlayedAnimation = false;
	}
}

static UWidgetAnimation* GetAnimation(UProperty* prop, void* PropertyAddress)
{
	// Run through all properties of this class to find any widget animations
	while (prop != nullptr)
	{
		// Only interested in object properties
		if (prop->GetClass() == UObjectProperty::StaticClass())
		{
			UObjectProperty* objectProp = Cast<UObjectProperty>(prop);

			// Only want the properties that are widget animations
			if (objectProp->PropertyClass == UWidgetAnimation::StaticClass())
			{
				UObject* object = objectProp->GetObjectPropertyValue_InContainer(PropertyAddress);

				UWidgetAnimation* widgetAnim = Cast<UWidgetAnimation>(object);

				if (widgetAnim != nullptr)
				{
					return widgetAnim;
				}
			}
		}

		prop = prop->PropertyLinkNext;
	}

	return nullptr;
}
