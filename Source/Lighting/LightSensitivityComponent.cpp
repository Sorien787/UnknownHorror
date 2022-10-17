// Fill out your copyright notice in the Description page of Project Settings.


#include "Lighting/LightSensitivityComponent.h"


void ULightSensitivityComponent::OnEnterLightArea()
{
	m_EnterLightAreaDelegate.Broadcast();
}

void ULightSensitivityComponent::OnExitLightArea()
{
	m_ExitLightAreaDelegate.Broadcast();
}

