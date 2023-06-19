// Fill out your copyright notice in the Description page of Project Settings.


#include "Base/BaseWeaponActor.h"
#include "DrawDebugHelpers.h"

static void SetCollision();

ABaseWeaponActor::ABaseWeaponActor()
{
	ABLOG(Warning, TEXT("ABaseWeaponActor Initialized"));

	PrimaryActorTick.bCanEverTick = false;

	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("TRIGGER"));

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	Mesh->SetCollisionProfileName(TEXT("NoCollision"));

	RootComponent = Trigger;
	Mesh->SetupAttachment(Trigger);

	bIsEquiped = false;
}

void ABaseWeaponActor::BeginPlay()
{
	Super::BeginPlay();
/*
#if ENABLE_DRAW_DEBUG
	Trigger->bHiddenInGame = false;
#endif
*/

	auto currentRotator = Trigger->GetRelativeRotation();
	auto bound = Mesh->SkeletalMesh->GetImportedBounds().BoxExtent;
	bound.X *= 2.f;
	Trigger->SetBoxExtent(bound);

	//auto temp = FRotationMatrix::Make(currentRotator.GetInverse()).TransformVector(bound);
	//Mesh->SetRelativeLocation(FVector(0.f, 0.f, -temp.Z));

	//Trigger->SetWorldRotation(FRotator::ZeroRotator);
	//Mesh->SetRelativeLocation(FVector(0.f, 0.f, -bound.Z * 0.5f));
	//Trigger->SetWorldRotation(currentRotator);

	// PickUp 가능여부 초기화
	if (!OwnPlayer)
	{
		Trigger->SetCollisionProfileName(TEXT("PhysicsActor"));
		Trigger->SetSimulatePhysics(true);
		Trigger->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		Trigger->OnComponentHit.AddDynamic(this, &ABaseWeaponActor::OnHit);
	}
	else
	{
		Trigger->SetCollisionProfileName(TEXT("NoCollision"));
	}
}

void ABaseWeaponActor::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void ABaseWeaponActor::OnCharacterOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Overlap 된 Actor가 BaseCharacter 인 경우 PickUp 호출
	ABLOG(Warning, TEXT("Overlapped"));
	auto Player = Cast<ABaseCharacter>(OtherActor); /* 필기: Cast 진행시 내부적으로 IsA 호출됨 */
	if (Player != nullptr)
	{
		PickUp(Player);
	}
}

void ABaseWeaponActor::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	// Overlap 된 Actor가 BaseCharacter 인 경우 PickUp 호출
	ABLOG(Warning, TEXT("Hitted"));
	auto Player = Cast<ABaseCharacter>(OtherActor); /* 필기: Cast 진행시 내부적으로 IsA 호출됨 */
	if (Player != nullptr)
	{
		PickUp(Player);
	}
}

void ABaseWeaponActor::PickUp(ABaseCharacter* character)
{
	auto bCanPickUp = character->CanPickup(this);
	if (bCanPickUp)
	{
		OwnPlayer = character;

		Trigger->SetSimulatePhysics(false);	/* 필기: 순서 중요 */
		Trigger->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Trigger->SetCollisionProfileName(TEXT("NoCollision"));
		Trigger->OnComponentHit.Clear();

		OwnPlayer->PickUpWeapon(this);
	}
}

void ABaseWeaponActor::ThrowWeapon()
{
	OwnPlayer = nullptr;

	Trigger->SetCollisionProfileName(TEXT("PhysicsActor"));
	Trigger->SetSimulatePhysics(true);
	Trigger->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	// 일정 시간 뒤에 Collision Event 적용되도록 설정
	FTimerHandle WaitHandle;
	GetWorld()->GetTimerManager().SetTimer(
		WaitHandle,
		FTimerDelegate::CreateLambda(
			[&] ()
			{
				Trigger->OnComponentHit.AddDynamic(this, &ABaseWeaponActor::OnHit);
			}
		), 0.5f, false);
}

void ABaseWeaponActor::OnEquip(const ABaseWeaponActor* LastWeapon)
{
	
}

void ABaseWeaponActor::OnUnEquip()
{

}

static void SetCollision()
{

}