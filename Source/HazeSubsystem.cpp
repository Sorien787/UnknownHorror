// Fill out your copyright notice in the Description page of Project Settings.


#include "HazeSubsystem.h"
#include "DrawDebugHelpers.h"
#include "HazeComponent.h"


void UHazeSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

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
