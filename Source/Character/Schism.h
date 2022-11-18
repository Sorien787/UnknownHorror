// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LegManager.h"
#include "WallClimbingMovementComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Haze/HazeComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Schism.generated.h"

UCLASS()
class DEEPSEAHORROR_API ASchism : public APawn
{
	GENERATED_BODY()
	
	FQuaternionSpringState m_RotSpringState;
	
	FQuat GetDesiredBodyRotation(float DeltaTime);

	bool MakeLineTrace(FVector start, FVector end, FHitResult& hitResult);

	struct AngleTestResult
	{
	public:
		AngleTestResult(bool hit, float desiredAng)
		{
			wasHitTerrain = hit;
			desiredAngle = desiredAng;
		}
		float desiredAngle{0.0f};
		bool wasHitTerrain{false};
	};
	
	AngleTestResult DoAngleTest(FVector startA, FVector endA, FVector startB, FVector endB, float defaultLength, float angleCheckDistanceStart, float angleCheckDistanceFinish);
public:	
	// Sets default values for this actor's properties
	ASchism();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UWallClimbingMovementComponent* m_pMovementComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	ULegManager* m_pLegManager;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UBoxComponent* m_pBoxComponentForLegManager;
	
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UHazeComponent* m_pHazeSource;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Animation | Body Rotation Parameters")
	float m_LegAlignmentBodyRotationYawSensitivity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Animation | Body Rotation Distances")
	float m_ForwardPredictionLengthMax;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Animation | Body Rotation Distances")
	float m_ForwardPredictionLengthMin;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Animation | Body Rotation Distances")
	float m_UpDownPredictionLengthMax;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Animation | Body Rotation Distances")
	float m_UpDownPredictionLengthMin;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Animation | Body Rotation Distances")
	float m_LeftRightPredictionLengthMax;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Animation | Body Rotation Distances")
	float m_LeftRightPredictionLengthMin;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Animation | Body Rotation Parameters")
	FRuntimeFloatCurve m_TurnSpeedByAngularOffsetToDesired;
};
