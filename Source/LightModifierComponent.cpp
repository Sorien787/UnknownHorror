// Fill out your copyright notice in the Description page of Project Settings.


#include "LightModifierComponent.h"

// Sets default values for this component's properties
ULightModifierComponent::ULightModifierComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void ULightModifierComponent::BeginPlay()
{
	Super::BeginPlay();
	// light is polled 
}

void ULightModifierComponent::SwitchOn()
{
	if(m_IsOn)
		return;
	SetComponentTickEnabled(true);
	SetLightIntensity(1.0f);
}

void ULightModifierComponent::SwitchOff()
{
	if (!m_IsOn)
		return;
	SetComponentTickEnabled(false);
	SetLightIntensity(0.0f);
	m_IsFlickering = false;
}

void ULightModifierComponent::SetLightIntensity(float intensity)
{
	for (auto pLightComponent : m_pLightComponentsArray)
	{
		pLightComponent->SetIntensity(intensity * m_DefaultBrightness);
	}
}

void ULightModifierComponent::AddLightToControlGroup(ULightComponent* pLightComponent)
{
	m_pLightComponentsArray.Add(pLightComponent);
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

