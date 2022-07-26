// Fill out your copyright notice in the Description page of Project Settings.


#include "LightHazeActor.h"

// Sets default values
ALightHazeActor::ALightHazeActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	m_pHazeEffectComponent = CreateDefaultSubobject<UHazeEffectComponent>(TEXT("Haze Component"));
	check(m_pHazeEffectComponent != nullptr);

	m_pLightModifier = CreateDefaultSubobject<ULightModifierComponent>(TEXT("Light Modifier Component"));
	check(m_pLightModifier != nullptr);
	

	m_LightPercentOnlineByHazeModifier.EditorCurveData.AddKey(0.0f, 1.0f);
	m_LightPercentOnlineByHazeModifier.EditorCurveData.AddKey(1.0f, 0.0f);

	m_LightFlickerFrequencyByHazeModifier.EditorCurveData.AddKey(0.0f, 1.0f);
	m_LightFlickerFrequencyByHazeModifier.EditorCurveData.AddKey(1.0f, 1.0f);
	
	m_LightMinBrightness.EditorCurveData.AddKey(0.0f, 0.5f);
	m_LightMinBrightness.EditorCurveData.AddKey(1.0f, 0.5f);
	
	m_LightMaxBrightness.EditorCurveData.AddKey(0.0f, 0.5f);
	m_LightMaxBrightness.EditorCurveData.AddKey(1.0f, 0.5f);
}

void ALightHazeActor::OnHazeStrengthChanged(float value)
{
	m_pLightModifier->m_LightFlickerFrequency = m_LightFlickerFrequencyByHazeModifier.EditorCurveData.Eval(value);
	m_pLightModifier->m_MinFlickerBrightness = m_LightMinBrightness.EditorCurveData.Eval(value);
	m_pLightModifier->m_MaxFlickerBrightness = m_LightMaxBrightness.EditorCurveData.Eval(value);
	m_pLightModifier->m_LightDesiredPercentOnline = m_LightPercentOnlineByHazeModifier.EditorCurveData.Eval(value);
}

void ALightHazeActor::OnHazeSetValue(float value)
{
	const bool bAttemptPop = value > m_fHazeModifierForPop;
	if (bAttemptPop && !m_bHasPopAttempted)
	{
		if (m_fHazeModifierForPop < m_pHazeEffectComponent->GetCurrentHazeModifier())
		{
			m_pHazeEffectComponent->m_HazeComponentListeners.RemoveListener(this);
			m_pLightModifier->SwitchOff();
			RunLightBreakAnim();
		}
	}
	m_bHasPopAttempted = bAttemptPop;
}




// Called when the game starts or when spawned
void ALightHazeActor::BeginPlay()
{
	Super::BeginPlay();
	m_pHazeEffectComponent->m_HazeComponentListeners.AddListener(this, "Light Listener");

}

void ALightHazeActor::BeginDestroy()
{
	Super::BeginDestroy();
	m_pHazeEffectComponent->m_HazeComponentListeners.RemoveListener(this);
}
