// Fill out your copyright notice in the Description page of Project Settings.



#include "HazeSubsystem.h"
#include "DrawDebugHelpers.h"
#include "HazeComponent.h"

void UHazeSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
}

void UHazeSubsystem::Tick(float deltaTime)
{
	std::unique_lock<std::mutex> lock(m_Mutex);

	m_UpdateLoopConditionVariable.wait(lock, std::bind(&UHazeSubsystem::HasHazeUpdateFinished, this));
	
	for (int hazeGridIndex = 0; hazeGridIndex < m_HazeGrids.size(); ++hazeGridIndex)
	{
		AHazeGridActor* pHazeGrid = m_HazeGrids[hazeGridIndex];
		pHazeGrid->SyncChangesToWorkingGrid();
	}
	m_bHasHazeUpdateFinished = false;
	
	if (m_UpdateLoopThread.joinable())
		m_UpdateLoopThread.join();
	
	m_UpdateLoopThread = std::thread(&UHazeSubsystem::ThreadUpdateLoop, this, deltaTime);
}

TStatId UHazeSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UHazeSubsystem, STATGROUP_Tickables);
}

void UHazeSubsystem::ThreadUpdateLoop(float deltaTime)
{
	for (int hazeGridIndex = 0; hazeGridIndex < m_HazeGrids.size(); ++hazeGridIndex)
	{
		AHazeGridActor* pHazeGrid = m_HazeGrids[hazeGridIndex];
		pHazeGrid->UpdateDiffusion(deltaTime);
	}

	m_bHasHazeUpdateFinished = true;
	m_UpdateLoopConditionVariable.notify_all();
}

bool UHazeSubsystem::HasHazeUpdateFinished() const
{
	return m_bHasHazeUpdateFinished;
}

UHazeSubsystem::~UHazeSubsystem()
{
	if (m_UpdateLoopThread.joinable())
		m_UpdateLoopThread.join();
}

void UHazeSubsystem::RegisterHazeGrid(AHazeGridActor* hazeGrid)
{
	const auto it = std::find(m_HazeGrids.begin(), m_HazeGrids.end(), hazeGrid);
	if (it != m_HazeGrids.end())
		return;
	m_HazeGrids.push_back(hazeGrid);
}

void UHazeSubsystem::UnregisterHazeGrid(AHazeGridActor* hazeGrid)
{
	const auto it = std::find(m_HazeGrids.begin(), m_HazeGrids.end(), hazeGrid);
	if (it == m_HazeGrids.end())
		return;
	m_HazeGrids.erase(it);
}

float UHazeSubsystem::PollHazeStrengthAtLocation(FVector strengthAtLocation, int hazeGridID) const
{
	return m_HazeGrids[hazeGridID]->SampleGrid(strengthAtLocation);
}

void UHazeSubsystem::AddHazeSourceAtLocation(FVector strengthAtLocation, float strength, int hazeGridID)
{
	m_HazeGrids[hazeGridID]->AddSampleToGrid(strengthAtLocation, strength);
}

int UHazeSubsystem::GetHazeIDAtLocation(FVector newLocation, int hazeGridID) const
{
	if (hazeGridID != -1)
	{
		if (m_HazeGrids[hazeGridID]->GridContainsPoint(newLocation))
			return hazeGridID;
	}

	for (int i = 0; i < m_HazeGrids.size(); i++)
	{
		if (hazeGridID == i)
			continue;
		
		const AHazeGridActor* pHazeGrid = m_HazeGrids[i];
		if (pHazeGrid->GridContainsPoint(newLocation))
			return i;
	}

	return -1;
}
