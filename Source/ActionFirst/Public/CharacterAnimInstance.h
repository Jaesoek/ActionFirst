// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ActionFirst.h"
#include "Animation/AnimInstance.h"
#include "CharacterAnimInstance.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnAttackHitCheck);
DECLARE_MULTICAST_DELEGATE(FOnActorDead);
DECLARE_MULTICAST_DELEGATE(FOnActorDestroy);

/**
 * 
 */
UCLASS()
class ACTIONFIRST_API UCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UCharacterAnimInstance();
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	void PlayAttackMontage();
	void PlayDeadMontage();

	void JumpToAttackMontageSection(int32 NewSection);

public:
	FOnAttackHitCheck OnAttackHitCheck;
	FOnActorDead OnActorDead;
	FOnActorDestroy OnActorDestroy;

	void SetDeadAnim() { IsDead = true; }

private:
	FName GetAttackMontageSectionName(int32 Section);

private:
	UFUNCTION()
	void AnimNotify_AttackHitCheck();

	UFUNCTION()
	void AnimNotify_ActorDead();

	UFUNCTION()
	void AnimNotify_ActorDestroy();

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn, Meta = (AllowPrivateAccess = true))
	float CurrentPawnSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn, Meta = (AllowPrivateAccess = true))
	bool IsDead;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	UAnimMontage* AttackMontage;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Dead, Meta = (AllowPrivateAccess = true))
	UAnimMontage* DeadMontage;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Weapon, Meta = (AllowPrivateAccess = true))
	EWeaponType CurrentWeaponType;
};
