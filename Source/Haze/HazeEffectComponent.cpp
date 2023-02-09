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
	const float perSecEventProbability = m_HazeStrengthToEventProbability.EditorCurveData.Eval(m_CurrentHazeStrength);
	const float eventProbability = perSecEventProbability * deltaTime;
	if (FMath::FRandRange(0.0f, 1.0f) < eventProbability)
	{
		m_OnHazeEvent.Broadcast();
		m_HazeComponentListeners.Notify(&HazeComponentListener::OnHazeEvent);
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
	
	m_HazeNoisePollLocation += deltaTime * m_HazeStrengthToNoiseFrequency.EditorCurveData.Eval(m_CurrentHazeStrength);
}

float UHazeEffectComponent::ConvertHazeValueToMultiplier() const
{
	return m_HazeStrengthToNoiseAmplitude.EditorCurveData.Eval(m_CurrentHazeStrength);
}

void UHazeEffectComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UHazeEffectComponent::RefreshHazeSink()
{
	const UWorld* pWorld = GetWorld();
	if (!pWorld)
		return;
	
	const UHazeSubsystem* hazeSubsystem = pWorld->GetSubsystem<UHazeSubsystem>();
	
	if (!hazeSubsystem)
		return;

	if (m_HazeID >= 0)
	{
		m_CurrentHazeStrength = hazeSubsystem->PollHazeStrengthAtLocation(m_LastPolledLocation, m_HazeID);}
	
	const FVector currentLocation = GetOwner()->GetActorLocation();
	
	if (FVector::DistSquared(currentLocation, m_LastPolledLocation) < m_distanceGranularity * m_distanceGranularity)
		return;

	
	OnRefreshHazeGridPosition();
}

void UHazeEffectComponent::OnRefreshHazeGridPosition()
{
	const UWorld* pWorld = GetWorld();
	if (!pWorld)
		return;
	
	const UHazeSubsystem* hazeSubsystem = pWorld->GetSubsystem<UHazeSubsystem>();
	
	if (!hazeSubsystem)
		return;

	const FVector currentLocation = GetOwner()->GetActorLocation();
	
	m_HazeID = hazeSubsystem->GetHazeIDAtLocation(currentLocation, m_HazeID);

	m_LastPolledLocation = currentLocation;

	if (m_HazeID != -1)
		return;
	
	m_CurrentHazeStrength = 0.0f;
}

void UHazeEffectComponent::OnItemControlGranted_Implementation(AActor* pControlledActor)
{
}

void UHazeEffectComponent::OnItemControlLost_Implementation(AActor* pControlledActor)
{
}

int UHazeEffectComponent::GetPriority() const
{
	return m_ItemControlPriority;
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

float UHazeEffectComponent::GetCurrentHazeModifier(int index /* = 0 */)
{
	if (m_HazeIdToSeed.find(index) == m_HazeIdToSeed.end())
	{
		m_HazeIdToSeed[index] = FMath::FRandRange(-10000.0f, 10000.0f);
	}
	return ConvertHazeValueToMultiplier() * FMath::PerlinNoise1D(m_HazeNoisePollLocation + m_HazeIdToSeed[index]);
}

float UHazeEffectComponent::GetCurrentHazeStrength() const
{
	return m_CurrentHazeStrength;	
}

float UHazeEffectComponent::GetNoiseFrequency() const
{
	return m_CurrentHazeStrength;	
}

