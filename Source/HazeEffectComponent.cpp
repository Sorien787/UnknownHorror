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

	// ...
}


void UHazeEffectComponent::UpdateHazeMultiplierValue(float deltaTime)
{
	float newMultiplier = ConvertHazeValueToMultiplier();
	bool hazeEffectActive = newMultiplier > 0.0f;

	bool hazeActiveChanged = hazeEffectActive != m_HazeReachedThreshold;
	
	m_HazeReachedThreshold = hazeEffectActive;


	if (hazeActiveChanged && m_HazeReachedThreshold)
	{
		m_HazeComponentListeners.Notify(&HazeComponentListener::OnHazeStart);
	}
	else if (hazeActiveChanged && !m_HazeReachedThreshold)
	{
		m_HazeComponentListeners.Notify(&HazeComponentListener::OnHazeFinish);
	}

	if (!m_HazeReachedThreshold)
		return;
	const float perlinVal =  FMath::PerlinNoise1D(m_HazeNoisePollLocation);
	m_HazeNoisePollLocation += deltaTime * FMath::Lerp(m_noiseFrequencyScale, m_noiseFrequencyScaleMax, newMultiplier);
	m_CurrentHazeModifier = perlinVal * newMultiplier;
	m_HazeComponentListeners.Notify(&HazeComponentListener::OnHazeSetValue, m_CurrentHazeModifier );
}

float UHazeEffectComponent::ConvertHazeValueToMultiplier() const
{
	return FMath::Clamp((m_CurrentHazeStrength - m_minimumHazeValue )/ (m_maximumHazeValue - m_minimumHazeValue), 0.0f, 1.0f);
}

// Called when the game starts
void UHazeEffectComponent::BeginPlay()
{
	Super::BeginPlay();
	const UWorld* world = GetWorld();
	if(!world)
		return;
	UHazeSubsystem* hazeSubsystem = world->GetSubsystem<UHazeSubsystem>();
	if (!hazeSubsystem)
		return;
	hazeSubsystem->AddHazeListener(this);
	OnRefreshHazeStrength();
	
}

void UHazeEffectComponent::BeginDestroy()
{
	Super::BeginDestroy();
	const UWorld* world = GetWorld();
	if(!world)
		return;
	
	UHazeSubsystem* hazeSubsystem = world->GetSubsystem<UHazeSubsystem>();
	
	if (!hazeSubsystem)
		return;
	hazeSubsystem->RemoveHazeListener(this);
}

void UHazeEffectComponent::OnRefreshHazeStrength()
{
	UWorld* pWorld = GetWorld();
	if (!pWorld)
		return;
	const UHazeSubsystem* hazeSubsystem = pWorld->GetSubsystem<UHazeSubsystem>();
	if (!hazeSubsystem)
		return;
	m_CurrentHazeStrength = hazeSubsystem->PollHazeStrengthAtLocation(GetOwner()->GetActorLocation());
	m_LastPolledLocation = GetOwner()->GetActorLocation();
}

// Called every frame
void UHazeEffectComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	UpdateHazeMultiplierValue(DeltaTime);
	const FVector currentLocation = GetOwner()->GetActorLocation();
	if (FVector::DistSquared(currentLocation, m_LastPolledLocation) < m_distanceGranularity * m_distanceGranularity)
		return;
	OnRefreshHazeStrength();
	// ...
}

bool UHazeEffectComponent::IsHazeActive() const
{
	return m_HazeReachedThreshold;
}

float UHazeEffectComponent::GetCurrentHazeModifier() const
{
	return m_CurrentHazeModifier;
}

