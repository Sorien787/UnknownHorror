// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIAlertLevel.h"
#include "Components/ActorComponent.h"
#include "PerceptionTypeComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DEEPSEAHORROR_API UPerceptionTypeComponent : public UActorComponent
{
	GENERATED_BODY()

	UPerceptionTypeComponent();

	UPROPERTY(EditAnywhere)
	EAIAlertLevel m_MaxAlertLevel{EAIAlertLevel::IDLE};
	
	UPROPERTY(EditAnywhere)
	uint8 m_Priority{0};

	UPROPERTY(EditAnywhere)
	float m_MaxInterest{0.0f};

	UPROPERTY(EditAnywhere)
	float m_VisualPerceptionModifier{10.0f};
	
	UPROPERTY(EditAnywhere)
	FRuntimeFloatCurve m_VisualFalloffModifierByDistance;
	
	UPROPERTY(EditAnywhere)
	FRuntimeFloatCurve m_AudioFalloffModifierByDistance;
	
	UPROPERTY(EditAnywhere)
	FRuntimeFloatCurve m_UncertaintyDistanceByCurrentSuspicion;

	UPROPERTY(EditAnywhere)
	FRuntimeFloatCurve m_PerceptionDecay;
	
public:
	
	EAIAlertLevel GetMaximumAlertLevel() const;

	float GetMaximumInterest() const;
	
	uint8 GetPriority() const;

	const float GetVisualPerceptionModifier() const;

	const float GetVisualPerceptionDistanceScalar(float currentInterestNormalized) const;

	const float GetAudioPerceptionDistanceScalar(float currentInterestNormalized) const;
	
	const float GetUncertaintyDistance(float currentInterestNormalized) const;

	const float GetPerceptionDecay(float currentInterestNormalized) const;
};
