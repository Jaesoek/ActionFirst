// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ActionFirst.h"
#include "Base/BaseCharacter.h"
#include "UserCharacter.generated.h"

class UCrossHairWidget;

DECLARE_MULTICAST_DELEGATE(FOnMouseTurn);

UCLASS()
class ACTIONFIRST_API AUserCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	AUserCharacter();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	virtual void PostInitializeComponents() override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void PossessedBy(AController* NewController) override;

private:
	virtual void UpDown(float NewAxisValue);
	virtual void LeftRight(float NewAxisValue);
	virtual void Turn(float NewAxisValue);
	virtual void LookUp(float NewAxisValue);
	virtual void CloseUp(float NewAxisValue);

	virtual void Dash();

	virtual void ForcePull();
	virtual void ForcePush();

	virtual void StartGrappling();

public:
	FOnMouseTurn OnMouseTurn;

public:
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
		AController* EventInstigator, AActor* DamageCauser) override;

	virtual void Attack() override;

	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

public:
	// For my pos RangeSkill
	UFUNCTION()
	void RangeSkillStart();

	UFUNCTION()
	void RangeSkillCasting(float time);

	UFUNCTION()
	void RangeSkillEnd();

public:
	// For my pos RangeSkill
	UFUNCTION()
	void RangeSkillStart_Far();

	UFUNCTION()
	void RangeSkillCasting_Far();

	UFUNCTION()
	void RangeSkillEnd_Far();

private:
	UPROPERTY(VisibleAnyWhere, Category = Camera)
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnyWhere, Category = Camera)
	UCameraComponent* Camera;

	UPROPERTY(VisibleAnyWhere, Category = Camera)
	UChildActorComponent* CameraChild;

	UPROPERTY(VisibleAnyWhere, Category = Camera)
	USpringArmComponent* GunSpringArm;

	UPROPERTY(VisibleAnyWhere, Category = Camera)
	UCameraComponent* GunCamera;

	UPROPERTY(VisibleAnyWhere, Category = Camera)
	UChildActorComponent* GunCameraChild;

	float ArmLengthTo = 0.f;
	FRotator ArmRotationTo = FRotator::ZeroRotator;
	float ArmLengthSpeed = 0.f;
	float ArmRotationSpeed = 0.f;


private:
	virtual void PickUpWeapon(class ABaseWeaponActor* weapon) override;
	virtual void ThrowWeapon() override;

	virtual void EquipWeapon() override;
	virtual void UnEquipWeapon() override;

private:
	// For Dash
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Stat", Meta = (AllowPrivateAccess = true))
	float CoolDown_Dash = 0.f;

private:
	UPROPERTY()
	class UCharacterAnimInstance* CharAnim;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Force Object", Meta = (AllowPrivateAccess = true))
	class AActor* FocusedActor;

	UPrimitiveComponent* FocusedComponent;

	UFUNCTION(BlueprintCallable, Category = "UserCharacter Function")
	void AttachMesh();

private:
	// For crosshair UI
	TSubclassOf<UUserWidget> CrosshairWidget_Class;

	UPROPERTY()
	UCrossHairWidget* CrosshairWidget;

private:
	UPROPERTY(VisibleAnywhere, Category = DecalMaterial)
	class UDecalComponent* RangeDecalComponent;

	UPROPERTY(EditAnywhere, Category = DecalMaterial)
	class UMaterial* M_Decal;
	class UMaterialInstanceDynamic* MID_Decal;

	bool IsCating = false;
	float CastingTime = 0.f;
	
	UPROPERTY(EditAnywhere, Category = SkillSetting, Meta = (AllowPrivateAccess = true))
	float RangeSkillCastingTime;

	UPROPERTY(EditAnywhere, Category = SkillSetting, Meta = (AllowPrivateAccess = true))
	float RangeSkillRange;

private:
	// For range check
	class ADecalActor* CheckRangeDecalActor;

	UPROPERTY(EditAnywhere, Category = DecalRangeCheck)
	class UMaterial* M_RangeCheck;
	class UMaterialInstanceDynamic* MID_RangeCheck;

private:
	// For grappling
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Grappling, Meta = (AllowPrivateAccess = true))
	FVector GrapplingEndPoint;

private:
	FCollisionQueryParams IgnoreOwner;
};
