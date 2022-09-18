// Fill out your copyright notice in the Description page of Project Settings.



#include "HazeSubsystem.h"
#include "DrawDebugHelpers.h"
#include "HazeComponent.h"


// start up a new thread
// have the thread update the haze grid every frame
// and we await the thread before, to be sure its done processing
// this is the same as single threaded...
// so make two forms of data: working, and 
void UHazeSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
}

void UHazeSubsystem::OnWorldUpdate(UWorld& InWorld)
{

	std::unique_lock<std::mutex> lock(m_Mutex);
	// Start waiting for the Condition Variable to get signaled
	// Wait() will internally release the lock and make the thread to block
	// As soon as condition variable get signaled, resume the thread and
	// again acquire the lock. Then check if condition is met or not
	// If condition is met then continue else again go in wait.
	m_UpdateLoopConditionVariable.wait(lock, std::bind(&UHazeSubsystem::HasHazeUpdateFinished, this));

	for (int hazeGridIndex = 0; hazeGridIndex < m_HazeGrids.size(); ++hazeGridIndex)
	{
		AHazeGridActor* pHazeGrid = m_HazeGrids[hazeGridIndex];
		pHazeGrid->SyncChangesToWorkingGrid();
	}
	m_bHasHazeUpdateFinished = false;
	m_UpdateThread(&UHazeSubsystem::ThreadUpdateLoop, this);

}

void UHazeSubsystem::ThreadUpdateLoop(float deltaTime)
{
	for (int hazeGridIndex = 0; hazeGridIndex < m_HazeGrids.size(); ++hazeGridIndex)
	{
		AHazeGridActor* pHazeGrid = m_HazeGrids[hazeGridIndex];
		pHazeGrid->UpdateDiffusionCycle(deltaTime);
	}

	m_bHasHazeUpdateFinished = true;

	m_UpdateLoopConditionVariable.notify_all();
}

void UHazeSubsystem::HasHazeUpdateFinished() const
{
	return m_bHasHazeUpdateFinished;
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
