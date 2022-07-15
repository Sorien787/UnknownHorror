// Fill out your copyright notice in the Description page of Project Settings.

#pragma optimize("", off)
#include "LightModifierComponent.h"

#include "Components/PointLightComponent.h"

// Sets default values for this component's properties
ULightModifierComponent::ULightModifierComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void ULightModifierComponent::BeginPlay()
{
	Super::BeginPlay();
	SetComponentTickEnabled(false);

	TArray<ULightComponent*> Lights;
	GetOwner()->GetComponents<ULightComponent>(Lights);

	m_IsFlickering = false;
	if (m_DefaultOn)
		SwitchOn(true);
	else
		SwitchOff(true);

	for (int i = 0; i < Lights.Num(); i++)
	{
		AddLightToControlGroup(Lights[i]);		
	}
}

void ULightModifierComponent::SwitchOn(bool force /* = false */)
{
	if(m_IsOn && !force)
		return;
	m_IsOn = true;
	SetComponentTickEnabled(true);
	SetLightIntensity(1.0f);
}

void ULightModifierComponent::SwitchOff(bool force /* = false */)
{
	if (!m_IsOn && !force)
		return;
	m_IsOn = false;
	SetComponentTickEnabled(false);
	SetLightIntensity(0.0f);
	m_IsFlickering = false;
}

void ULightModifierComponent::SetLightIntensity(float intensity)
{
	m_CurrentIntensity = intensity;
	for (auto pLightComponent : m_pLightComponentsArray)
	{
		pLightComponent->SetIntensity(intensity * m_DefaultBrightness);
	}
}

void ULightModifierComponent::OnHazeStrengthChanged(float hazeStrength)
{
	// ideally, haze multiplies instability
	// so we have an instability float that is a "dial" for a given light
}

void ULightModifierComponent::AddLightToControlGroup(ULightComponent* pLightComponent)
{
	m_pLightComponentsArray.Add(pLightComponent);
	pLightComponent->SetIntensity(m_CurrentIntensity * m_DefaultBrightness);
}

void ULightModifierComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	float transitionAttemptProbability;

	if (m_IsFlickering)
		transitionAttemptProbability = FMath::FRandRange(0.0f, m_LightDesiredPercentOnline);
	else
		transitionAttemptProbability = FMath::FRandRange(0.0f, 1-m_LightDesiredPercentOnline);
	
	if(transitionAttemptProbability < FMath::FRandRange(0.0f, 1.0f))
		return;
	// m_LightFlickerFrequency

	m_IsFlickering = !m_IsFlickering;

	if (m_IsFlickering)
	{
		SetLightIntensity(FMath::FRandRange(m_MinFlickerBrightness, m_MaxFlickerBrightness));	
	}
	else
	{
		SetLightIntensity(1.0f);
	}
}

