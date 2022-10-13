// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LightModifierFlickerStruct.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FLightFlickerStateStruct
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float m_MinFlickerBrightness;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float m_MaxFlickerBrightness;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float m_LightFlickerFrequency;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float m_LightDesiredPercentOnline;

	FLightFlickerStateStruct();

	FLightFlickerStateStruct(float min, float brightness, float freq, float proportion);
};
