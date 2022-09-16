// Fill out your copyright notice in the Description page of Project Settings.

#include "HazeGridActor.h"
#include "HazeSubsystem.h"


AHazeGridActor::AHazeGridActor()
{
	PrimaryActorTick.bCanEverTick = false;

	m_HazeGrid = DiffusionGrid(valueForMaxDebug, gridExtents.X, gridExtents.Y, gridExtents.Z, GetActorLocation().X,
							   GetActorLocation().Y, GetActorLocation().Z, gridElementSize, diffusionCoefficient,
							   drainageCoefficient);
}

void AHazeGridActor::BeginPlay()
{
	Super::BeginPlay();

	UWorld* world = GetWorld();
	if (!world)
		return;
	
	UHazeSubsystem* hazeSubsystem = world->GetSubsystem<UHazeSubsystem>();
	if (!hazeSubsystem)
		return;
	
	hazeSubsystem->RegisterHazeGrid(this);
}

void AHazeGridActor::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	// just regenerate the whole heckin grid, it's not that big
	m_HazeGrid = DiffusionGrid(valueForMaxDebug, gridExtents.X, gridExtents.Y, gridExtents.Z, GetActorLocation().X,
							   GetActorLocation().Y, GetActorLocation().Z, gridElementSize, diffusionCoefficient,
							   drainageCoefficient);
}

void AHazeGridActor::BeginDestroy()
{
	Super::BeginDestroy();

	UWorld* world = GetWorld();
	if (!world)
		return;
	
	UHazeSubsystem* hazeSubsystem = world->GetSubsystem<UHazeSubsystem>();
	if (!hazeSubsystem)
		return;
	
	hazeSubsystem->UnregisterHazeGrid(this);
}

bool AHazeGridActor::GridContainsPoint(FVector point) const
{
	return m_HazeGrid.IsWorldInputWithinGrid(point);
}

float AHazeGridActor::SampleGrid(FVector point) const
{
	return m_HazeGrid.SampleGrid(point.X, point.Y, point.Z);
}

void AHazeGridActor::AddSampleToGrid(FVector point, float sample)
{
	m_HazeGrid.AddDiffusionSource(point.X, point.Y, point.Z, sample);
}

