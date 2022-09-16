// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HazeEffectComponent.h"
#include "PressureGaugeActor.generated.h"

UCLASS()
class DEEPSEAHORROR_API APressureGaugeActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APressureGaugeActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintImplementableEvent)
	void OnSetPressureGaugeAngle(float angle);

	UFUNCTION(BlueprintImplementableEvent)
	void OnPressureGaugeExplode(float angle);

	UPROPERTY(EditAnywhere)
	UHazeEffectComponent* m_pHazeEffector;

	UPROPERTY(EditAnywhere)
	float m_fDefaultAngle;
	
	UPROPERTY(EditAnywhere)
	float m_fExplodeProbability;

	UPROPERTY(EditAnywhere)
	float m_fMinHazeForExplode;

	UPROPERTY(EditAnywhere)
	FRuntimeFloatCurve m_HazeStrengthToAcceleration;

	UPROPERTY(EditAnywhere)
	float m_SpringContant;

	UPROPERTY(EditAnywhere)
	float m_SpringDamping;

	UPROPERTY(EditAnywhere)
	float m_VelocityForUncontrolledSpin;


	
	UPROPERTY(EditAnywhere)
	float m_fMaxAngle;
	
	float m_fCurrentVelocity;

	float m_fCurrentAngle;
	

};
