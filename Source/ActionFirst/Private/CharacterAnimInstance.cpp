// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterAnimInstance.h"
#include "Base/BaseCharacter.h"
#include "Base/BaseWeaponActor.h"

UCharacterAnimInstance::UCharacterAnimInstance()
{
	CurrentPawnSpeed = 0.f;
	IsDead = false;
	CurrentWeaponType = EWeaponType::None;

	static ConstructorHelpers::FObjectFinder<UAnimMontage> ATTACK_MONTAGE(
		TEXT("/Game/OwnAssets/Animations/SK_Mannequin_Skeleton_Montage.SK_Mannequin_Skeleton_Montage")
	);
	if (ATTACK_MONTAGE.Succeeded())
	{
		AttackMontage = ATTACK_MONTAGE.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> Dead_MONTAGE(
		TEXT("/Game/OwnAssets/Animations/WarriorDead_Montage.WarriorDead_Montage")
	);
	if (Dead_MONTAGE.Succeeded())
	{
		DeadMontage = Dead_MONTAGE.Object;
	}
}

void UCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	auto Pawn = TryGetPawnOwner();
	if (!::IsValid(Pawn)) return;

	CurrentPawnSpeed = Pawn->GetVelocity().Size();

	// Weapon Type
	auto weapon = Cast<ABaseCharacter>(Pawn)->CurrentWeapon;
	if (weapon != nullptr) CurrentWeaponType = weapon->weaponType;
	else CurrentWeaponType = EWeaponType::None;
}

void UCharacterAnimInstance::PlayAttackMontage()
{
	Montage_Play(AttackMontage, 1.f);
}

void UCharacterAnimInstance::PlayDeadMontage()
{
	Montage_Play(DeadMontage, 1.f);
}

void UCharacterAnimInstance::AnimNotify_AttackHitCheck()
{
	OnAttackHitCheck.Broadcast();
}

void UCharacterAnimInstance::AnimNotify_ActorDead()
{
	OnActorDead.Broadcast();
}

void UCharacterAnimInstance::AnimNotify_ActorDestroy()
{
	OnActorDestroy.Broadcast();
}

FName UCharacterAnimInstance::GetAttackMontageSectionName(int32 Section)
{
	return FName(*FString::Printf(TEXT("Attack%d"), Section));
}

void UCharacterAnimInstance::JumpToAttackMontageSection(int32 NewSection)
{
	ABCHECK(!IsDead);
	Montage_Play(AttackMontage, 1.f);
	Montage_JumpToSection(GetAttackMontageSectionName(NewSection), AttackMontage);
}
