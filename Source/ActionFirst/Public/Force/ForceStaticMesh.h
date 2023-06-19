// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ActionFirst.h"
#include "GameFramework/Actor.h"
#include "ForceStaticMesh.generated.h"

/* For sound */
UENUM()
enum class MaterialType :uint8
{
	Wood,
	Rock,
	Metal
};

/**
 * TODO: 붙잡혔는지는 Owner로 확인하는 식으로 진행
 */
UCLASS(Abstract)
class ACTIONFIRST_API AForceStaticMesh : public AActor
{
	GENERATED_BODY()

public:
	AForceStaticMesh();

protected:
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ExposeOnSpawn = true, AllowPrivateAccess = true))
	UStaticMeshComponent* Mesh;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Object Stat", Meta = (AllowPrivateAccess = true))
	float Mass = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Object Stat", Meta = (AllowPrivateAccess = true))
	MaterialType Type;

public:
	bool GetIsFocused() { return bIsFocused; };
	void SetFocused();

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Focusing", Meta = (AllowPrivateAccess = true))
	bool bIsFocused = false;
};
