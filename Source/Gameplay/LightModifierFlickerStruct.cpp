// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/LightModifierFlickerStruct.h"

FLightFlickerStateStruct::FLightFlickerStateStruct()
{
}

FLightFlickerStateStruct::FLightFlickerStateStruct(float min, float brightness, float freq, float proportion)
{
	m_MaxFlickerBrightness = brightness;
	m_MinFlickerBrightness = min;
	m_LightFlickerFrequency = freq;
	m_LightDesiredPercentOnline = proportion;
}
