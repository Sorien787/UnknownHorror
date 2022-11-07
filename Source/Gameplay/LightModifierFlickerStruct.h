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
	float m_MinFlickerBrightness {0.0f};

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float m_MaxFlickerBrightness{0.0f};;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float m_LightFlickerFrequency{0.0f};;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float m_LightDesiredPercentOnline{0.0f};;

	FLightFlickerStateStruct();

	FLightFlickerStateStruct(float min, float brightness, float freq, float proportion);
};
