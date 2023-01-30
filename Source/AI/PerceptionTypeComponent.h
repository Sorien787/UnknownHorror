// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIAlertLevel.h"
#include "Components/ActorComponent.h"
#include "PerceptionTypeComponent.generated.h"


USTRUCT()
struct FPerceptionTypeModifier
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	float m_PerceptionModifier;
	
	UPROPERTY(EditAnywhere)
	float m_PerceptionDecayModifier;

	UPROPERTY(EditAnywhere)
	float m_TimeBeforePerceptionDecayBegin;
		
	UPROPERTY(EditAnywhere)
	FRuntimeFloatCurve m_NewPerceptionFalloffModifierByDistance;
	
	UPROPERTY(EditAnywhere)
	FRuntimeFloatCurve m_PerceptionDecayMultiplierByCurrentPerception;

	UPROPERTY(EditAnywhere)
	FRuntimeFloatCurve m_PerceptionDecayMultiplierByLifetime;
	
	UPROPERTY(EditAnywhere)
	EAIAlertLevel m_MaxAlertLevel{EAIAlertLevel::IDLE};
	
	UPROPERTY(EditAnywhere)
	float m_MaxInterest{0.0f};
};

UENUM()
enum class ESenseEnum : uint8
{
	Default,
	Sight,
	Hearing,
	Touch,
	COUNT
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DEEPSEAHORROR_API UPerceptionTypeComponent : public UActorComponent
{
	GENERATED_BODY()

	UPerceptionTypeComponent();



	UPROPERTY(EditAnywhere)
	uint8 m_Priority{0};


	UPROPERTY(EditAnywhere)
	TMap<ESenseEnum, FPerceptionTypeModifier> m_PerceptionTypeMap;
	
	UPROPERTY(EditAnywhere)
	bool m_CanBeIgnored{false};
public:

	uint8 GetPriority() const;

	bool GetCanBeIgnored() const;

	const FPerceptionTypeModifier* GetPerceptionTypeModifierForSense(FAISenseID senseID) const;
};
