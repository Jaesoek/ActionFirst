// Fill out your copyright notice in the Description page of Project Settings.


#include "UserCharacter.h"
#include "Base/BaseWeaponActor.h"
#include "CharacterAnimInstance.h"
#include "Widget/CrossHairWidget.h"
#include "DrawDebugHelpers.h"
#include "MainPlayerController.h"
#include "Camera/CameraActor.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Engine/DecalActor.h"
#include "Components/DecalComponent.h"

// Sets default values
AUserCharacter::AUserCharacter()
{
	ABLOG(Warning, TEXT("UserCharacter"));

	// Initialize CameraObejects
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraChild = CreateDefaultSubobject<UChildActorComponent>(TEXT("CameraChild"));
	CameraChild->SetChildActorClass(ACameraActor::StaticClass());

	GunSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("GunSpringArm"));
	GunCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("GunCamera"));
	GunCameraChild = CreateDefaultSubobject<UChildActorComponent>(TEXT("GunCameraChild"));
	GunCameraChild->SetChildActorClass(ACameraActor::StaticClass());

	// Attach Main Camera and set base location, rotation
	SpringArm->SetupAttachment(GetCapsuleComponent());
	SpringArm->TargetArmLength = 600.f;
	SpringArm->SetRelativeRotation(FRotator(-30.f, 0.f, 0.f));
	Camera->SetupAttachment(SpringArm);
	CameraChild->SetupAttachment(Camera);

	// Attach Gun Camera
	GunSpringArm->SetupAttachment(GetCapsuleComponent());
	GunSpringArm->TargetArmLength = 210.f;
	GunSpringArm->SetRelativeLocation(FVector(0.f, 70.f, 42.f));
	GunCamera->SetupAttachment(GunSpringArm);
	GunCameraChild->SetupAttachment(GunCamera);

	// Load Skeletal
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_PLAYER_MESH(
		TEXT("/Game/InfinityBladeWarriors/Character/CompleteCharacters/SK_CharM_Bladed.SK_CharM_Bladed"));
	if (SK_PLAYER_MESH.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(SK_PLAYER_MESH.Object);
	}
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.f, -88.f), FRotator(0.0f, -90.f, 0.f));

	// Set animation
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	static ConstructorHelpers::FClassFinder<UAnimInstance> WARRIOR_ANIM(
		TEXT("/Game/OwnAssets/Animations/WarriorAnimBlueprint.WarriorAnimBlueprint_C")
	);
	if (WARRIOR_ANIM.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(WARRIOR_ANIM.Class);
	}

	// Load Crosshair ui
	static ConstructorHelpers::FClassFinder<UUserWidget> UI_CrossHair(
		TEXT("/Game/OwnAssets/UI/UHD_CrossHair.UHD_Crosshair_C")
	);
	if (UI_CrossHair.Succeeded())
	{
		CrosshairWidget_Class = UI_CrossHair.Class;
	}

	// Hp setting
	MaxHp = 200.f;
	CurrentHp = MaxHp;

	// Range skill setting
	RangeSkillCastingTime = 3.f;
	RangeSkillRange = 256.f;

	// Init decal component
	RangeDecalComponent = CreateDefaultSubobject<UDecalComponent>("RangeSkillDecal");
	RangeDecalComponent->SetRelativeLocation(FVector(0.f, 0.f, -88.f));
	RangeDecalComponent->DecalSize = FVector(88.0f, RangeSkillRange, RangeSkillRange);
	RangeDecalComponent->SetRelativeRotation(FRotator(-90.f, 0.f, 0.f).Quaternion());
	RangeDecalComponent->SetupAttachment(GetCapsuleComponent());

	// Set decal settings
	GetMesh()->bReceivesDecals = false;
	RangeDecalComponent->SetVisibility(false);
	RangeDecalComponent->bHiddenInGame = true;
}

// Called when the game starts or when spawned
void AUserCharacter::BeginPlay()
{
	Super::BeginPlay();
	ABLOG(Warning, TEXT("UserCharacter BeginPlay"));

	IgnoreOwner = FCollisionQueryParams::DefaultQueryParam;
	IgnoreOwner.AddIgnoredActor(this);
}

// Called every frame
void AUserCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// TODO: 현재 들고 있는 경우에는 무시하기
	// Focus Actor 리스트
	auto&& actorLocation = GetActorLocation();
	TArray<struct FHitResult> results;
	bool isHit = GetWorld()->SweepMultiByObjectType(
		results,
		actorLocation,
		actorLocation + Camera->GetForwardVector() * 2000.f,
		FQuat::Identity,
		ECollisionChannel::ECC_PhysicsBody,
		FCollisionShape::MakeSphere(400.f),
		IgnoreOwner
	);

	if (isHit)
	{
		// TODO: 마우스 커서 위치값에 기반하여 처리를 해줘야하는데.. 일단은 가까운거만 Focus 맞춰지는 걸로
		//  포커스 변경
		if (FocusedActor != results[0].GetActor())
		{
			if (FocusedComponent != nullptr)
			{
				FocusedComponent->SetRenderCustomDepth(false);
			}

			FocusedComponent = results[0].GetComponent();
			FocusedActor = results[0].GetActor();
			FocusedComponent->SetRenderCustomDepth(true);
		}
	}
	else
	{
		// Focus Actor 제거
		if (FocusedComponent != nullptr)
		{
			FocusedComponent->SetRenderCustomDepth(false);
		}
		FocusedActor = nullptr;
		FocusedComponent = nullptr;
	}

	// CoolDown
	CoolDown_Dash -= DeltaTime;

	// Casting Skill
	if (IsCating)
	{
		RangeSkillCasting(DeltaTime);
	}
}

void AUserCharacter::PostInitializeComponents()
{
	ABLOG(Warning, TEXT("UserCharacter PostInitializeComponents Begin"));
	Super::PostInitializeComponents();
	ABLOG(Warning, TEXT("UserCharacter PostInitializeComponents End"));

	CharAnim = Cast<UCharacterAnimInstance>(GetMesh()->GetAnimInstance());
	CharAnim->OnMontageEnded.AddDynamic(this, &AUserCharacter::OnAttackMontageEnded);

	// Init MIDs for range skill
	if (M_Decal != nullptr) MID_Decal = UMaterialInstanceDynamic::Create(M_Decal, this);
	RangeDecalComponent->SetDecalMaterial(MID_Decal);

	if (M_RangeCheck != nullptr) MID_RangeCheck = UMaterialInstanceDynamic::Create(M_RangeCheck, this);

	// Level editor에서 출력되지 않도록 하기 위해
	RangeDecalComponent->SetVisibility(true);
}

// Called to bind functionality to input
void AUserCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	ABLOG(Warning, TEXT("AUserCharacter SetupPlayerInputComponent Begin"));
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	ABLOG(Warning, TEXT("AUserCharacter SetupPlayerInputComponent End"));

	PlayerInputComponent->BindAxis(TEXT("UpDown"), this, &AUserCharacter::UpDown);
	PlayerInputComponent->BindAxis(TEXT("LeftRight"), this, &AUserCharacter::LeftRight);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &AUserCharacter::Turn);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &AUserCharacter::LookUp);
	PlayerInputComponent->BindAxis(TEXT("CloseUp"), this, &AUserCharacter::CloseUp);

	PlayerInputComponent->BindAction(TEXT("Attack"), EInputEvent::IE_Pressed, this, &AUserCharacter::Attack);
	PlayerInputComponent->BindAction(TEXT("Attack_Hard"), EInputEvent::IE_Pressed, this, &AUserCharacter::Attack);
	PlayerInputComponent->BindAction(TEXT("Dash"), EInputEvent::IE_Pressed, this, &AUserCharacter::Dash);
	PlayerInputComponent->BindAction(TEXT("ThrowWeapon"), EInputEvent::IE_Pressed, this, &AUserCharacter::ThrowWeapon);
	PlayerInputComponent->BindAction(TEXT("Force"), EInputEvent::IE_Pressed, this, &AUserCharacter::ForcePull);
	PlayerInputComponent->BindAction(TEXT("Force"), EInputEvent::IE_Released, this, &AUserCharacter::ForcePush);

	PlayerInputComponent->BindAction(TEXT("RangeSkill"), EInputEvent::IE_Pressed, this, &AUserCharacter::RangeSkillStart);
	PlayerInputComponent->BindAction(TEXT("RangeSkill"), EInputEvent::IE_Released, this, &AUserCharacter::RangeSkillEnd);
	PlayerInputComponent->BindAction(TEXT("RangeSkillFar"), EInputEvent::IE_Pressed, this, &AUserCharacter::RangeSkillStart_Far);
	PlayerInputComponent->BindAction(TEXT("RangeSkillFar"), EInputEvent::IE_Released, this, &AUserCharacter::RangeSkillEnd_Far);

	//	PlayerInputComponent->BindAction(TEXT("Grappling"), EInputEvent::IE_Pressed, this, &AUserCharacter::StartGrappling);
}

void AUserCharacter::PossessedBy(AController* NewController)
{
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->bInheritRoll = true;
	SpringArm->bInheritPitch = true;
	SpringArm->bInheritYaw = true;

	GunSpringArm->bUsePawnControlRotation = true;
	GunSpringArm->bInheritRoll = true;
	GunSpringArm->bInheritPitch = true;
	GunSpringArm->bInheritYaw = true;

	// Character control setting
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	// Basic property
	GetCharacterMovement()->MaxWalkSpeed = 600.f;
}

void AUserCharacter::UpDown(float NewAxisValue)
{
	if (NewAxisValue != 0.f)
	{
		const FRotator Rotation = GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		AddMovementInput(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X), NewAxisValue);
	}
}

void AUserCharacter::LeftRight(float NewAxisValue)
{
	if (NewAxisValue != 0.f)
	{
		AddMovementInput(FRotationMatrix(GetControlRotation()).GetUnitAxis(EAxis::Y), NewAxisValue);
	}
}

void AUserCharacter::Turn(float NewAxisValue)
{
	if (NewAxisValue != 0.f)
	{
		AddControllerYawInput(NewAxisValue);
		OnMouseTurn.Broadcast();
	}
}

void AUserCharacter::LookUp(float NewAxisValue)
{
	if (NewAxisValue != 0.f)
	{
		if (CurrentWeapon == nullptr || CurrentWeapon->weaponType == EWeaponType::Sword)
			AddControllerPitchInput(NewAxisValue * 0.5f);
		else
			AddControllerPitchInput(NewAxisValue * 0.5f);
	}
}

void AUserCharacter::CloseUp(float NewAxisValue)
{
	if (CurrentWeapon == nullptr || CurrentWeapon->weaponType == EWeaponType::Sword)
		SpringArm->TargetArmLength -= NewAxisValue * 50.f;
}

void AUserCharacter::Dash()
{
	if (CoolDown_Dash > 0.f) return;
	else CoolDown_Dash = 3.0f;

	// 바라보는 방향으로 Dash 실행
	LaunchCharacter(GetActorForwardVector() * 5000.f, false, false);
}

void AUserCharacter::ForcePull()
{
	if (FocusedComponent != nullptr)
	{
		ABLOG(Warning, TEXT("ForcePull actived! %d"), FocusedComponent);

		FocusedComponent->SetSimulatePhysics(false);
		// TODO: Collision 설정 변경해주기

		auto socketLocation = GetMesh()->GetSocketLocation(FName(TEXT("rhand_farSocket")));

		FLatentActionInfo info;
		info.CallbackTarget = this;
		info.ExecutionFunction = FName(TEXT("AttachMesh"));
		info.Linkage = 0;
		info.UUID = 0;

		UKismetSystemLibrary::MoveComponentTo(
			FocusedComponent,
			socketLocation,
			FRotator::ZeroRotator,
			true,
			false,
			0.2f,
			false,
			EMoveComponentAction::Move,
			info
		);

		FocusedActor->SetOwner(this);

		// Release Focus
		FocusedComponent = nullptr;
		FocusedActor = nullptr;
	}
}

void AUserCharacter::ForcePush()
{
}

void AUserCharacter::Attack()
{
	if (CurrentWeapon == nullptr) return;
	switch (CurrentWeapon->weaponType)
	{
		case EWeaponType::Gun:
		{
			if (CurrentWeapon != nullptr && CurrentWeapon->weaponType == EWeaponType::Gun)
			{
				// Animation 실행
				CrosshairWidget->PlayAnimation(CrosshairWidget->Fire);
			}

			FHitResult result;
			bool isHit = GetWorld()->LineTraceSingleByChannel(
				result,
				GetActorLocation() + GetActorRotation().RotateVector(FVector(0.f, 70.f, 42.f)),
				GetActorLocation() + GetActorRotation().RotateVector(FVector(0.f, 70.f, 42.f)) + GunCameraChild->GetForwardVector() * 100000,
				ECollisionChannel::ECC_GameTraceChannel2,
				IgnoreOwner
			);

			if (isHit)
			{
				if (result.Actor.IsValid())
				{
					FVector ShotDirection = result.ImpactNormal;
					TSubclassOf <UDamageType> DamageType;

					// Apply damage
					FPointDamageEvent PointDamageEvent{ 100.f, result, ShotDirection, DamageType };
					result.GetActor()->TakeDamage(100.f, PointDamageEvent, GetController(), this);
				}
			}

			break;
		}
		case EWeaponType::Sword:
		{
			if (isAttacking) return;
			isAttacking = true;

			// Play Animation
			CharAnim->PlayAttackMontage();

			FHitResult result;
			bool isHit = GetWorld()->SweepSingleByChannel(
				result,
				GetActorLocation(),
				GetActorLocation() + GetActorForwardVector() * 200,
				FQuat::Identity,
				ECollisionChannel::ECC_GameTraceChannel2,
				FCollisionShape::MakeSphere(100.f),
				IgnoreOwner
			);

			if (isHit)
			{
				if (result.Actor.IsValid())
				{
					result.GetActor()->TakeDamage(20.f, FDamageEvent{}, GetController(), this);
				}
			}
			break;
		}
	}
}

// For range attack
void AUserCharacter::RangeSkillStart()
{
	if (RangeDecalComponent == nullptr) return;

	IsCating = true;
	CastingTime = 0.f;
	RangeDecalComponent->SetHiddenInGame(false);
}

void AUserCharacter::RangeSkillCasting(float time)
{
	if (RangeDecalComponent == nullptr) return;

	CastingTime += time / RangeSkillCastingTime;
	if (CastingTime >= 1.0f)
	{
		RangeSkillEnd();
		CharAnim->JumpToAttackMontageSection(4);

		TArray<FHitResult> result;
		bool isHit = GetWorld()->SweepMultiByChannel(
			result,
			GetActorLocation(),
			GetActorLocation(),
			FQuat::Identity,
			ECollisionChannel::ECC_GameTraceChannel2,
			FCollisionShape::MakeSphere(256.f),
			IgnoreOwner
		);

		TArray<AActor*> attackedActors;
		if (isHit)
		{
			for (auto hitResult : result)
			{
				if (hitResult.Actor.IsValid() && !attackedActors.Contains(hitResult.GetActor()))
				{
					attackedActors.Add(hitResult.GetActor());
					hitResult.GetActor()->TakeDamage(100.f, FDamageEvent{}, GetController(), this);
				}
			}
		}
	}
	else
	{
		if (MID_Decal != nullptr)
		{
			MID_Decal->SetScalarParameterValue(FName(TEXT("Angle")), CastingTime);
			MID_Decal->SetScalarParameterValue(FName(TEXT("Percent")), CastingTime);
		}
	}
}

void AUserCharacter::RangeSkillEnd()
{
	if (RangeDecalComponent == nullptr) return;

	IsCating = false;
	CastingTime = 0.f;
	RangeDecalComponent->SetHiddenInGame(true);
}

// For range far attack
void AUserCharacter::RangeSkillStart_Far()
{
	// Spawn actor in here
	check(MID_RangeCheck);

	auto cameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	FHitResult result;
	bool isHit = GetWorld()->LineTraceSingleByObjectType(
		result,
		cameraManager->GetCameraLocation(),
		cameraManager->GetCameraLocation() + cameraManager->GetActorForwardVector() * 10000,
		ECollisionChannel::ECC_WorldStatic,
		IgnoreOwner
	);

	FVector location;
	if (isHit)
		location = result.ImpactPoint;
	else
		location = GetActorLocation();

	CheckRangeDecalActor = GetWorld()->SpawnActor<ADecalActor>(location, FRotator());
	if (CheckRangeDecalActor)
	{
		CheckRangeDecalActor->SetDecalMaterial(MID_RangeCheck);
		CheckRangeDecalActor->GetDecal()->DecalSize = FVector(600.0f, 600.0f, 600.0f);
	}

	// 
	OnMouseTurn.AddUObject(this, &AUserCharacter::RangeSkillCasting_Far);
}

UFUNCTION()
void AUserCharacter::RangeSkillCasting_Far()
{
	if (CheckRangeDecalActor != nullptr)
	{
		auto cameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
		FHitResult result;
		bool isHit = GetWorld()->LineTraceSingleByObjectType(
			result,
			cameraManager->GetCameraLocation(),
			cameraManager->GetCameraLocation() + cameraManager->GetActorForwardVector() * 4000,
			ECollisionChannel::ECC_WorldStatic,
			IgnoreOwner
		);
		
		bool isAvailable = true;

		FVector location;
		FRotator pitchRotation = FRotator{ -90.f, 0.f, 0.f };
		if (isHit)
		{ // 사정거리 내부
			location = result.ImpactPoint;
			pitchRotation = FRotationMatrix::MakeFromX(result.ImpactNormal).Rotator();

			if (FVector::DotProduct(FVector::ZAxisVector, result.ImpactNormal) < 0.4f)
				isAvailable = false;
		}
		else
		{ // 사정거리 바깥
			isAvailable = false;
			location = GetActorLocation();
		}

		CheckRangeDecalActor->SetActorLocation(location);
		CheckRangeDecalActor->SetActorRotation(pitchRotation);
		if (isAvailable)
			MID_RangeCheck->SetScalarParameterValue(FName(TEXT("IsAvailable")), 1.f);
		else
			MID_RangeCheck->SetScalarParameterValue(FName(TEXT("IsAvailable")), 0.f);
	}
}

UFUNCTION()
void AUserCharacter::RangeSkillEnd_Far()
{
	// Spawn actor in there
	check(CheckRangeDecalActor);
	CheckRangeDecalActor->Destroy();
}

void AUserCharacter::StartGrappling()
{
	FHitResult result;
	bool isHit = GetWorld()->LineTraceSingleByChannel(
		result,
		GetActorLocation(),
		GetActorLocation() + GetActorForwardVector() * 10000,
		ECollisionChannel::ECC_GameTraceChannel2,
		IgnoreOwner
	);
	if (isHit)
	{
		if (result.Actor.IsValid())
		{
			GrapplingEndPoint = result.ImpactPoint;
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow,
				FString::Printf(TEXT("Damage! %f %f %f"), GrapplingEndPoint.X, GrapplingEndPoint.Y, GrapplingEndPoint.Z)
			);
			// TODO: Line 연결
			// -> 연결 완료시 연락받기

			// 1차는 우선 힘을 받기
		}
	}
}

float AUserCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

void AUserCharacter::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	AttackEnd();
}

void AUserCharacter::PickUpWeapon(ABaseWeaponActor* weapon)
{
	Super::PickUpWeapon(weapon);
}

void AUserCharacter::ThrowWeapon()
{
	Super::ThrowWeapon();
	UnEquipWeapon();
}

void AUserCharacter::EquipWeapon()
{
	Super::EquipWeapon();
	switch (CurrentWeapon->weaponType)
	{
		case EWeaponType::Sword:
		{
			auto playerController = Cast<AMainPlayerController>(GetController());
			playerController->SetViewTargetWithBlend(CameraChild->GetChildActor());

			if (CrosshairWidget != nullptr)
			{
				CrosshairWidget->RemoveFromParent();
				CrosshairWidget = nullptr;
			}

			bUseControllerRotationYaw = false;
			GetCharacterMovement()->bOrientRotationToMovement = true;
			break;
		}
		case EWeaponType::Gun:
		{
			auto playerController = Cast<AMainPlayerController>(GetController());
			playerController->SetViewTargetWithBlend(GunCameraChild->GetChildActor());
			
			CrosshairWidget = Cast<UCrossHairWidget>(CreateWidget(GetWorld(), CrosshairWidget_Class, TEXT("Crosshair")));
			CrosshairWidget->AddToViewport();

			bUseControllerRotationYaw = true;
			GetCharacterMovement()->bOrientRotationToMovement = false;
			break;
		}
	}
}

void AUserCharacter::UnEquipWeapon()
{
	if (CurrentWeapon == nullptr)
	{
		auto playerController = Cast<AMainPlayerController>(GetController());
		playerController->SetViewTargetWithBlend(CameraChild->GetChildActor());

		if (CrosshairWidget != nullptr)
		{
			CrosshairWidget->RemoveFromParent();
			CrosshairWidget = nullptr;
		}

		bUseControllerRotationYaw = false;
		GetCharacterMovement()->bOrientRotationToMovement = true;
	}
}

void AUserCharacter::AttachMesh()
{
	FName WeaponSocket(TEXT("rhand_farSocket"));
	FocusedActor->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocket);
}
