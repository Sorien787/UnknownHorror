// Fill out your copyright notice in the Description page of Project Settings.


#include "HazeSubsystem.h"

void UHazeSubsystem::AddHazeListener(HazeListener* hazeListener)
{
	m_Listeners.AddListener(hazeListener, "New Haze Listener");
	hazeListener->OnRefreshHazeStrength();
}

void UHazeSubsystem::RemoveHazeListener(HazeListener* hazeListener)
{
	m_Listeners.RemoveListener(hazeListener);
}

void UHazeSubsystem::RegisterHazeSource(UHazeComponent* hazeComponent)
{
	m_HazeSources.push_back(hazeComponent);
}

void UHazeSubsystem::UnregisterHazeSource(UHazeComponent* hazeComponent)
{
	const auto it = std::find(m_HazeSources.begin(), m_HazeSources.end(), hazeComponent);
	if (it == m_HazeSources.end())
		return;
	m_HazeSources.erase(it);
}

float UHazeSubsystem::PollHazeStrengthAtLocation(FVector worldLocation) const
{
	float totalHaze = 0.0f;
	for(const auto& hazeComponent : m_HazeSources)
	{
		totalHaze += hazeComponent->GetHazeFieldAtWorldLocation(worldLocation);
	}
	return totalHaze;
}

void UHazeSubsystem::OnHazeSourceUpdated()
{
	m_Listeners.Notify(&HazeListener::OnRefreshHazeStrength);
}