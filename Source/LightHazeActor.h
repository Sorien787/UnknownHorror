// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LightModifierComponent.h"
#include "GameFramework/Actor.h"
#include "LightHazeActor.generated.h"

UCLASS()
class DEEPSEAHORROR_API ALightHazeActor : public AActor, public HazeComponentListener
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALightHazeActor();

	virtual void OnHazeStrengthChanged(float value) override;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void OnHazeSetValue(float value) override;

	bool m_bHasPopAttempted = false;

public:

	UPROPERTY(EditAnywhere)
	float m_fPoppingProbability = 0.0f;

	UPROPERTY(EditAnywhere)
	float m_fHazeModifierForPop = 0.0f;

	UFUNCTION(BlueprintImplementableEvent)
	float RunLightBreakAnim();

	UPROPERTY(EditAnywhere)
	ULightModifierComponent* m_pLightModifier;

	UPROPERTY(EditAnywhere)
	UHazeEffectComponent* m_pHazeEffectComponent;

	UPROPERTY(EditAnywhere, Category = Setup)
	FRuntimeFloatCurve m_LightPercentOnlineByHazeModifier;

	UPROPERTY(EditAnywhere, Category = Setup)
	FRuntimeFloatCurve m_LightFlickerFrequencyByHazeModifier;

	UPROPERTY(EditAnywhere, Category = Setup)
	FRuntimeFloatCurve m_LightMinBrightness;

	UPROPERTY(EditAnywhere, Category = Setup)
	FRuntimeFloatCurve m_LightMaxBrightness;
};
