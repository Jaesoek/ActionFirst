// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ActionFirst.h"
#include "Base/BaseCharacter.h"
#include "EnemyCharacter.generated.h"

UCLASS()
class ACTIONFIRST_API AEnemyCharacter : public ABaseCharacter
{
	GENERATED_BODY()

private:
	friend class AEnemyAIController;

public:
	AEnemyCharacter();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	virtual void PostInitializeComponents() override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	// Hp bar
	UPROPERTY(VisibleAnywhere, Category = UI)
	class UWidgetComponent* HPBarWidget;

	// Notify Icon
	UPROPERTY(VisibleAnywhere, Category = UI)
	class UWidgetComponent* NotifyIcon;

public:
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
		AController* EventInstigator, AActor* DamageCauser) override;

	virtual void Attack() override;

	void Skill();

	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

private:
	UPROPERTY(VisibleAnywhere, Category = Animation)
	class UCharacterAnimInstance* CharAnim;

	UPROPERTY()
	class UMaterialInstanceDynamic* MatInstance;

	UPROPERTY(EditDefaultsOnly, Category = Animation)
	class UCurveFloat* CurveDead;

private:
	bool bIsDead = false;
	float DeadTime = 0.f;
};
