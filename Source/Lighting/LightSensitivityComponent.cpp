// Fill out your copyright notice in the Description page of Project Settings.


#include "Lighting/LightSensitivityComponent.h"

bool ULightSensitivityComponent::IsCurrentlyLit() const
{
	return numLightsInside > 0;
}

void ULightSensitivityComponent::OnEnterLightArea()
{
	numLightsInside += 1;
	
	if (numLightsInside > 1)
		return;
	
	m_EnterLightAreaDelegate.Broadcast();
}

void ULightSensitivityComponent::OnExitLightArea()
{
	numLightsInside -= 1;

	if (numLightsInside > 0)
		return;
	
	m_ExitLightAreaDelegate.Broadcast();
}

