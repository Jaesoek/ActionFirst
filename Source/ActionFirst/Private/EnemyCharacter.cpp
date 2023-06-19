// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyCharacter.h"
#include "EnemyAIController.h"
#include "CharacterAnimInstance.h"
#include "Components/WidgetComponent.h"
#include "Components/ProgressBar.h"
#include "Curve.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
AEnemyCharacter::AEnemyCharacter()
{
	ABLOG(Warning, TEXT("AEnemyCharacter"));

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

	// Widget Component Setting
	// Set HpBar
	HPBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HpBar"));
	HPBarWidget->SetupAttachment(GetMesh());
	HPBarWidget->SetRelativeLocation(FVector(0.f, 0.f, 160.f));
	HPBarWidget->SetWidgetSpace(EWidgetSpace::Screen);

	static ConstructorHelpers::FClassFinder<UUserWidget> UI_HUD(TEXT("/Game/OwnAssets/UI/HpBar.HpBar_C"));
	if (UI_HUD.Succeeded())
	{
		HPBarWidget->SetWidgetClass(UI_HUD.Class);
		HPBarWidget->SetDrawSize(FVector2D(120.f, 30.f));
	}

	// Set NotifyIcon
	NotifyIcon = CreateDefaultSubobject<UWidgetComponent>(TEXT("NotifyIcon"));
	NotifyIcon->SetupAttachment(GetMesh());
	NotifyIcon->SetRelativeLocation(FVector(0.f, 30.f, 180.f));
	NotifyIcon->SetWidgetSpace(EWidgetSpace::Screen);

	static ConstructorHelpers::FClassFinder<UUserWidget> UI_NOTIFY(TEXT("/Game/OwnAssets/UI/IconNotify.IconNotify_C"));
	if (UI_NOTIFY.Succeeded())
	{
		NotifyIcon->SetWidgetClass(UI_NOTIFY.Class);
		NotifyIcon->SetDrawSize(FVector2D(30.f, 30.f));
	}

	//Speed Set
	Cast<UCharacterMovementComponent>(GetMovementComponent())->MaxWalkSpeed = 200.f;

	// Hp setting
	MaxHp = 200.f;
	CurrentHp = MaxHp;

	// Set AI Controller
	AIControllerClass = AEnemyAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

// Called when the game starts or when spawned
void AEnemyCharacter::BeginPlay()
{
	ABLOG(Warning, TEXT("AEnemyCharacter BeginPlay Begin"));
	Super::BeginPlay();
	ABLOG(Warning, TEXT("AEnemyCharacter BeginPlay End"));

	auto controller = Cast<AEnemyAIController>(Controller);
	if (controller == nullptr)
	{
		ABLOG(Warning, TEXT("AI Controller doesn't inited"));
	}
	else
	{
		// TODO: AI 일단 잠금
		// controller->RunAI();
	}
}

// Called every frame
void AEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsDead)
	{
		DeadTime += DeltaTime;
		float value = CurveDead->GetFloatValue(DeadTime);
		if (MatInstance != nullptr)
		{
			MatInstance->SetScalarParameterValue(FName(TEXT("erode")), value);
		}
		else
		{
			ABLOG(Warning, TEXT("Wrong mat instance"));
		}
	}
}

void AEnemyCharacter::PostInitializeComponents()
{
	ABLOG(Warning, TEXT("AEnemyCharacter PostInitializeComponents Begin"));
	Super::PostInitializeComponents();
	ABLOG(Warning, TEXT("AEnemyCharacter PostInitializeComponents End"));

	// Create dynamic material
	auto material = GetMesh()->GetMaterial(0);
//	MatInstance = UMaterialInstanceDynamic::Create(material, this);
	MatInstance = GetMesh()->CreateDynamicMaterialInstance(0, material);

	// Set animation callback
	CharAnim = Cast<UCharacterAnimInstance>(GetMesh()->GetAnimInstance());
	CharAnim->OnMontageEnded.AddDynamic(this, &AEnemyCharacter::OnAttackMontageEnded);
	CharAnim->OnActorDead.AddLambda( [this] () {
		bIsDead = true;
	});
	CharAnim->OnActorDestroy.AddLambda([this]() {
		// Destroy character in level
		Destroy();
	});
}

// Called to bind functionality to input
void AEnemyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

float AEnemyCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float FinalDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	// Widget Component 적용
	auto widgetObject = HPBarWidget->GetUserWidgetObject();
	auto progress = Cast<UProgressBar>(widgetObject->GetWidgetFromName("H"));
	if (progress != nullptr)
	{
		float ratio = CurrentHp / MaxHp;
		progress->SetPercent(ratio);
	}

	// TODO: Dead Animation start
	if (CurrentHp <= 0.f)
	{
		CharAnim->PlayDeadMontage();
	}

	return FinalDamage;
}

void AEnemyCharacter::Attack()
{
	if (isAttacking) return;
	isAttacking = true;

	CharAnim->PlayAttackMontage();

	FCollisionQueryParams IgnoreOwner = FCollisionQueryParams::DefaultQueryParam;
	IgnoreOwner.AddIgnoredActor(this);

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
			result.GetActor()->TakeDamage(20.f, FDamageEvent{}, GetController(), this);
	}
}

void AEnemyCharacter::Skill()
{
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(this);
	TArray<AActor*> OutActors;

	auto isHitted = UKismetSystemLibrary::SphereOverlapActors(
		GetWorld(),
		GetActorLocation(),
		600.f,
		ObjectTypes,
		nullptr,
		IgnoreActors,
		OutActors
	);

	for (auto actor : OutActors)
	{
		const auto& targetVector = (actor->GetActorLocation() - GetActorLocation());
		auto dot = FVector::DotProduct(GetActorForwardVector(), targetVector.GetSafeNormal());
		if (dot > 0.5f)
		{
			actor->TakeDamage(20.f, FDamageEvent{}, GetController(), this);
		}
	}
}

void AEnemyCharacter::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	AttackEnd();
}