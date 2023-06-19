// Fill out your copyright notice in the Description page of Project Settings.


#include "Force/ForceStaticMesh.h"

AForceStaticMesh::AForceStaticMesh()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));

	RootComponent = Mesh;
}

void AForceStaticMesh::BeginPlay()
{
	Super::BeginPlay();
}

void AForceStaticMesh::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	Mesh->SetCollisionProfileName(TEXT("PhysicsActor"));
	Mesh->SetSimulatePhysics(true);
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void AForceStaticMesh::SetFocused()
{
}
