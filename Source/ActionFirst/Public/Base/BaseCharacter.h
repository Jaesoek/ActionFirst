// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ActionFirst.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"

/*
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnCharacterEquipWeapon, ABaseCharacter*, ABaseWeaponActor*);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnCharacterUnEquipWeapon, ABaseCharacter*, ABaseWeaponActor*);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnPickUpWeapon, ABaseCharacter*, ABaseWeaponActor*);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnThrowWeapon, ABaseCharacter*, ABaseWeaponActor*);
*/

class UNiagaraSystem;
class ABaseWeaponActor;

DECLARE_MULTICAST_DELEGATE(FOnAttackEndDelegate);

UCLASS()
class ACTIONFIRST_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	virtual void Attack();
	virtual void AttackEnd();
	FOnAttackEndDelegate OnAttackEnd;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	bool isAttacking;

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
		AController* EventInstigator, AActor* DamageCauser) override;

public:
	/*
	FOnPickUpWeapon OnPickUpWeapon;
	FOnThrowWeapon OnThrowWeapon;
	FOnCharacterEquipWeapon OnEquipWeapon;
	FOnCharacterUnEquipWeapon OnUnEquipWeapon;
	*/

	bool CanPickup(ABaseWeaponActor* weapon);
	virtual void PickUpWeapon(ABaseWeaponActor* weapon);
	virtual void ThrowWeapon();

	virtual void EquipWeapon();
	virtual void UnEquipWeapon();

	// For Weapon
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", Meta = (AllowPrivateAccess = true))
	ABaseWeaponActor* CurrentWeapon;

protected:
	// For Hp, Sp
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Stat", Meta = (AllowPrivateAccess = true))
	float MaxHp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Stat", Meta = (AllowPrivateAccess = true))
	float CurrentHp;

private:
	// For Damage Skin Widget
	TSubclassOf<class UDamageWidget> DamageWidget_Class;

	UPROPERTY(EditAnywhere, Category = "Firing")
	class UNiagaraSystem* BloodParticle;
};
