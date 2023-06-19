// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ActionFirst.h"
#include "GameFramework/Actor.h"
#include "BaseCharacter.h"
#include "BaseWeaponActor.generated.h"

UCLASS(Abstract, Blueprintable, BlueprintType)
class ACTIONFIRST_API ABaseWeaponActor : public AActor
{
	GENERATED_BODY()

friend class ABaseCharacter;

public:	
	// Sets default values for this actor's properties
	ABaseWeaponActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;

private:
	UFUNCTION()
	void OnCharacterOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);

private:
	UBoxComponent* Trigger;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ExposeOnSpawn = true, AllowPrivateAccess = true))
	USkeletalMeshComponent* Mesh;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EWeaponType weaponType;

protected:
	class ABaseCharacter* OwnPlayer;

private:
	bool bIsEquiped;

public:
	virtual void PickUp(class ABaseCharacter* character);
	virtual void ThrowWeapon();

	virtual void OnEquip(const ABaseWeaponActor* LastWeapon);
	virtual void OnUnEquip();

	bool IsEquiped() const { return bIsEquiped; };
};
