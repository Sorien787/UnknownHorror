// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LegManager.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Schism.generated.h"

UCLASS()
class DEEPSEAHORROR_API ASchism : public APawn
{
	GENERATED_BODY()
	
	FQuaternionSpringState m_RotSpringState;
	
	FQuat GetDesiredBodyRotation(float DeltaTime);

	void SetNewBodyRotation();
	
	void SetNewBodyHeight();
	
public:	
	// Sets default values for this actor's properties
	ASchism();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Test")
	float m_x;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Test")
    float m_y;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Test")
	float m_z;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	ULegManager* m_pLegManager;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	ULegComponent* m_pFrontLeftLeg;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	ULegComponent* m_pFrontRightLeg;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	ULegComponent* m_pBackLeftLeg;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	ULegComponent* m_pBackRightLeg;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	USceneComponent* m_pRootComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	USkeletalMeshComponent* m_pSkeletalMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation | Body Rotation")
	float m_BodyRotationSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation | Body Rotation")
	float m_BodyRotationDamping;
};
