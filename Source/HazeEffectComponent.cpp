// Fill out your copyright notice in the Description page of Project Settings.


#include "HazeEffectComponent.h"
#include "HazeSubsystem.h"

// Sets default values for this component's properties
UHazeEffectComponent::UHazeEffectComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UHazeEffectComponent::BeginPlay()
{
	Super::BeginPlay();
	const UWorld* world = GetWorld();
	UHazeSubsystem* hazeSubsystem = world->GetSubsystem<UHazeSubsystem>();
	hazeSubsystem->AddHazeListener(this);
	m_LastPolledLocation = GetOwner()->GetActorLocation();
	
}

void UHazeEffectComponent::BeginDestroy()
{
	Super::BeginDestroy();
	const UWorld* world = GetWorld();
	UHazeSubsystem* hazeSubsystem = world->GetSubsystem<UHazeSubsystem>();
	hazeSubsystem->RemoveHazeListener(this);
}

void UHazeEffectComponent::OnRefreshHazeStrength()
{
	UWorld* pWorld = GetWorld();
	const UHazeSubsystem* hazeSubsystem = pWorld->GetSubsystem<UHazeSubsystem>();
	m_CurrentHazeStrength = hazeSubsystem->PollHazeStrengthAtLocation(GetOwner()->GetActorLocation());
	m_LastPolledLocation = GetOwner()->GetActorLocation();
	OnHazeStrengthChanged(m_CurrentHazeStrength);
}

// Called every frame
void UHazeEffectComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	const FVector currentLocation = GetOwner()->GetActorLocation();
	if (FVector::DistSquared(currentLocation, m_LastPolledLocation) < m_distanceGranularity * m_distanceGranularity)
		return;
	OnRefreshHazeStrength();
	// ...
}

