// Fill out your copyright notice in the Description page of Project Settings.

#include "HazeGridActor.h"
#include "HazeSubsystem.h"

AHazeGridActor::AHazeGridActor()
{
	PrimaryActorTick.bCanEverTick = true;
	
	m_HazeGrid = DiffusionGrid(valueForMaxDebug, gridExtents.X, gridExtents.Y, gridExtents.Z, GetActorLocation().X,
						   GetActorLocation().Y, GetActorLocation().Z, gridElementSize, diffusionCoefficient,
						   drainageCoefficient);
	m_WorkingGrid = m_HazeGrid;
}

void AHazeGridActor::BeginPlay()
{
	Super::BeginPlay();

	m_HazeGrid = DiffusionGrid(valueForMaxDebug, gridExtents.X, gridExtents.Y, gridExtents.Z, GetActorLocation().X,
					   GetActorLocation().Y, GetActorLocation().Z, gridElementSize, diffusionCoefficient,
					   drainageCoefficient);
	m_WorkingGrid = m_HazeGrid;

	const UWorld* world = GetWorld();
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
	m_WorkingGrid = m_HazeGrid;
}

void AHazeGridActor::SyncChangesToWorkingGrid()
{
	m_HazeGrid = m_WorkingGrid;

	for (int nPendingChangeIndex = 0; nPendingChangeIndex < m_PendingChanges.size(); nPendingChangeIndex++)
	{
		const PendingChange& pendingChange = m_PendingChanges[nPendingChangeIndex];
		m_WorkingGrid.AddDiffusionSource(pendingChange.x, pendingChange.y, pendingChange.z, pendingChange.value);
	}
	
	m_PendingChanges.clear();
}

void AHazeGridActor::UpdateDiffusion(float deltaTime)
{
	m_WorkingGrid.RunDiffusionCycle(deltaTime);
}

void AHazeGridActor::Tick(float deltaTime)
{
	IConsoleVariable* pConsoleVariable = IConsoleManager::Get().RegisterConsoleVariable(TEXT("HazeGridDebug"),
	   0,
	   TEXT("Shows haze grid debug visualization.\n")
		TEXT("<=0: off\n")
		TEXT(">=1: on\n"),
	   ECVF_Scalability | ECVF_RenderThreadSafe);
	
	if (pConsoleVariable->GetInt() > 0)
		m_HazeGrid.DebugDrawGrid(GetWorld());
}

void AHazeGridActor::BeginDestroy()
{
	Super::BeginDestroy();

	const UWorld* world = GetWorld();
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
	m_PendingChanges.emplace_back(point.X, point.Y, point.Z, sample);
}

