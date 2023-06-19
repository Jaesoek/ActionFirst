// Fill out your copyright notice in the Description page of Project Settings.


#include "Base/BaseCharacter.h"
#include "Base/BaseWeaponActor.h"
#include "Widget/DamageWidget.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

// Sets default values
ABaseCharacter::ABaseCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Load Damage ui
	static ConstructorHelpers::FClassFinder<UUserWidget> UI_Damage(
		TEXT("/Game/OwnAssets/UI/UI_Damage.UI_Damage_C")
	);
	if (UI_Damage.Succeeded())
	{
		DamageWidget_Class = UI_Damage.Class;
	}

	CurrentWeapon = nullptr;
}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

float ABaseCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float FinalDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
	{
		auto PointDamageEvent = static_cast<const FPointDamageEvent*>(&DamageEvent);
		CurrentHp -= FinalDamage;

		auto impactPoint = PointDamageEvent->HitInfo.ImpactPoint;
		if (BloodParticle) {
			// This spawns the chosen effect on the owning WeaponMuzzle SceneComponent
			UNiagaraComponent* NiagaraComp =
				UNiagaraFunctionLibrary::SpawnSystemAttached(
					BloodParticle, GetMesh(), NAME_None, impactPoint, FRotator(0.f),
					EAttachLocation::Type::KeepWorldPosition, true
				);
		}
	}
	else
	{
		CurrentHp -= FinalDamage;
	}

	// Damage UI Ãâ·Â
	auto Damage = Cast<UDamageWidget>(CreateWidget(GetWorld(), DamageWidget_Class));
	Damage->setPlayer(GetController());
	Damage->setAmountDamage(FinalDamage);
	Damage->AddToViewport();

	return FinalDamage;
}

bool ABaseCharacter::CanPickup(ABaseWeaponActor* weapon)
{
	return CurrentWeapon == nullptr;
}

void ABaseCharacter::PickUpWeapon(ABaseWeaponActor* weapon)
{
	CurrentWeapon = weapon;

	EquipWeapon();
}

void ABaseCharacter::ThrowWeapon()
{
	if (CurrentWeapon != nullptr)
	{
		CurrentWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		CurrentWeapon->SetOwner(nullptr);
		CurrentWeapon->ThrowWeapon();

		CurrentWeapon = nullptr;
	}
}

void ABaseCharacter::EquipWeapon()
{
	if (CurrentWeapon->weaponType == EWeaponType::Gun)
	{
		// Set weapon
		FName WeaponSocket(TEXT("rhand_gunSocket"));
		CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocket);
	}
	else
	{
		// Set weapon
		FName WeaponSocket(TEXT("hand_rSocket"));
		CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocket);
	}

	CurrentWeapon->SetOwner(this);
}

void ABaseCharacter::UnEquipWeapon()
{
}

void ABaseCharacter::Attack()
{
}

void ABaseCharacter::AttackEnd()
{
	ABLOG(Warning, TEXT("Attack is end"));
	isAttacking = false;
	OnAttackEnd.Broadcast();
}
