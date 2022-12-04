// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/PerceptionTypeComponent.h"

// Sets default values for this component's properties
UPerceptionTypeComponent::UPerceptionTypeComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

EAIAlertLevel UPerceptionTypeComponent::GetMaximumAlertLevel() const
{
	return m_MaxAlertLevel;
}

float UPerceptionTypeComponent::GetMaximumInterest() const
{
	return m_MaxInterest;
}

uint8 UPerceptionTypeComponent::GetPriority() const
{
	return m_Priority;
}

const float UPerceptionTypeComponent::GetVisualPerceptionModifier() const
{
	return m_VisualPerceptionModifier;
}

const float UPerceptionTypeComponent::GetVisualPerceptionDistanceScalar(float currentInterestNormalized) const
{
	return m_VisualFalloffModifierByDistance.EditorCurveData.Eval(currentInterestNormalized);
}

const float UPerceptionTypeComponent::GetAudioPerceptionDistanceScalar(float currentInterestNormalized) const
{
	return m_AudioFalloffModifierByDistance.EditorCurveData.Eval(currentInterestNormalized);
}

const float UPerceptionTypeComponent::GetVisualDecayDelay() const
{
	return m_TimeBeforeVisualInterestDecayBegin;
}

const float UPerceptionTypeComponent::GetAudioDecayDelay() const
{
	return m_TimeBeforeAudioInterestDecayBegin;
}

const float UPerceptionTypeComponent::GetUncertaintyDistance(float currentInterestNormalized) const
{
	return m_UncertaintyDistanceByCurrentSuspicion.EditorCurveData.Eval(currentInterestNormalized);
}

const float UPerceptionTypeComponent::GetPerceptionDecay(float currentInterestNormalized) const
{
	return m_PerceptionDecay.EditorCurveData.Eval(currentInterestNormalized);
}

