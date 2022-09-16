// Fill out your copyright notice in the Description page of Project Settings.


#include "HazeComponent.h"
#include "HazeSubsystem.h"

void UHazeComponent::RefreshHazeSource()
{
	UWorld* world = GetWorld();

	if (!world)
		return;
	
	UHazeSubsystem* hazeSubsystem = world->GetSubsystem<UHazeSubsystem>();

	if (!hazeSubsystem)
		return;
	
	const FVector currentLocation = GetOwner()->GetActorLocation();

	if (m_HazeID >= 0)
	{
		hazeSubsystem->AddHazeSourceAtLocation(m_LastPolledLocation, m_HazeStrengthMultiplier, m_HazeID);
	}
	
	if (FVector::DistSquared(currentLocation, m_LastPolledLocation) < m_distanceGranularity * m_distanceGranularity)
		return;

	RefreshHazePosition();
}

UHazeComponent::UHazeComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UHazeComponent::BeginPlay()
{
	Super::BeginPlay();
	
	RefreshHazeSource();
}

void UHazeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	RefreshHazeSource();
}


void UHazeComponent::RefreshHazePosition()
{
	UWorld* world = GetWorld();

	if (!world)
		return;
	
	UHazeSubsystem* hazeSubsystem = world->GetSubsystem<UHazeSubsystem>();
	
	if (!hazeSubsystem)
		return;

	const FVector currentLocation = GetOwner()->GetActorLocation();
	
	m_LastPolledLocation = currentLocation;

	m_HazeID = hazeSubsystem->GetHazeIDAtLocation(currentLocation, m_HazeID);
}


