// Fill out your copyright notice in the Description page of Project Settings.


#include "HazeEffectComponent.h"

#include <string>

#include "HazeSubsystem.h"

// Sets default values for this component's properties
UHazeEffectComponent::UHazeEffectComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	m_HazeStrengthToNoiseFrequency.EditorCurveData.AddKey(0.0f, 1.0f);
	m_HazeStrengthToNoiseFrequency.EditorCurveData.AddKey(1.0f, 1.0f);

	m_HazeStrengthToNoiseAmplitude.EditorCurveData.AddKey(0.1f, 0.0f);
	m_HazeStrengthToNoiseAmplitude.EditorCurveData.AddKey(1.0f, 1.0f);
}


void UHazeEffectComponent::UpdateHazeMultiplierValue(float deltaTime)
{
	const float newMultiplier = ConvertHazeValueToMultiplier();
	
	const bool hazeEffectActive = newMultiplier > 0.0f;

	const bool hazeActiveChanged = hazeEffectActive != m_HazeReachedThreshold;
	
	m_HazeReachedThreshold = hazeEffectActive;

	const float eventProbability = m_HazeStrengthToEventProbability.EditorCurveData.Eval(m_CurrentHazeStrength) * deltaTime;
	if (FMath::FRandRange(0.0f, 100.0f) < eventProbability)
	{
		const UWorld* world = GetWorld();
		if(!world)
			return;
	
		UHazeSubsystem* hazeSubsystem = world->GetSubsystem<UHazeSubsystem>();
	
		if (!hazeSubsystem)
			return;

		m_OnHazeComponentBreak.Broadcast();
		SetComponentTickEnabled(false);
		return;
	}
	
	if (hazeActiveChanged && m_HazeReachedThreshold)
	{
		m_HazeComponentListeners.Notify(&HazeComponentListener::OnHazeStart);
		m_OnHazeStart.Broadcast();
	}
	else if (hazeActiveChanged && !m_HazeReachedThreshold)
	{
		m_OnHazeFinish.Broadcast();
		m_HazeComponentListeners.Notify(&HazeComponentListener::OnHazeFinish);
	}

	if (!m_HazeReachedThreshold)
		return;
	const float perlinVal =  FMath::PerlinNoise1D(m_HazeNoisePollLocation + m_RandomSeed);
	m_HazeNoisePollLocation += deltaTime * m_HazeStrengthToNoiseFrequency.EditorCurveData.Eval(m_CurrentHazeStrength);
	m_CurrentHazeModifier = perlinVal * newMultiplier;
	m_HazeComponentListeners.Notify(&HazeComponentListener::OnHazeSetValue, m_CurrentHazeModifier );
	m_OnHazeModifierChanged.Broadcast(m_CurrentHazeModifier);
}

float UHazeEffectComponent::ConvertHazeValueToMultiplier() const
{
	return m_HazeStrengthToNoiseAmplitude.EditorCurveData.Eval(m_CurrentHazeStrength);
}

void UHazeEffectComponent::RefreshHazeSink()
{
	
	UWorld* pWorld = GetWorld();
	if (!pWorld)
		return;
	
	const UHazeSubsystem* hazeSubsystem = pWorld->GetSubsystem<UHazeSubsystem>();
	
	if (!hazeSubsystem)
		return;

	if (m_HazeID >= 0)
	{
		m_CurrentHazeStrength = hazeSubsystem->PollHazeStrengthAtLocation(m_LastPolledLocation, m_HazeID);
	
		m_HazeComponentListeners.Notify(&HazeComponentListener::OnHazeStrengthChanged, m_CurrentHazeStrength );
	}
	
	const FVector currentLocation = GetOwner()->GetActorLocation();
	
	if (FVector::DistSquared(currentLocation, m_LastPolledLocation) < m_distanceGranularity * m_distanceGranularity)
		return;

	
	OnRefreshHazeGridPosition();
}

void UHazeEffectComponent::BeginPlay()
{
	Super::BeginPlay();
	
	m_RandomSeed = FMath::FRandRange(-10000.0f, 10000.0f);
	
	RefreshHazeSink();
}


void UHazeEffectComponent::OnRefreshHazeGridPosition()
{
	UWorld* pWorld = GetWorld();
	if (!pWorld)
		return;
	
	const UHazeSubsystem* hazeSubsystem = pWorld->GetSubsystem<UHazeSubsystem>();
	
	if (!hazeSubsystem)
		return;

	const FVector currentLocation = GetOwner()->GetActorLocation();
	
	m_HazeID = hazeSubsystem->GetHazeIDAtLocation(currentLocation, m_HazeID);

	m_LastPolledLocation = currentLocation;
}

void UHazeEffectComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	UpdateHazeMultiplierValue(DeltaTime);
	
	RefreshHazeSink();
}

bool UHazeEffectComponent::IsHazeActive() const
{
	return m_HazeReachedThreshold;
}

float UHazeEffectComponent::GetCurrentHazeModifier() const
{
	return m_CurrentHazeModifier;
}

float UHazeEffectComponent::GetCurrentHazeStrength() const
{
	return m_CurrentHazeStrength;	
}

float UHazeEffectComponent::GetNoiseFrequency() const
{
	return m_CurrentHazeStrength;	
}

