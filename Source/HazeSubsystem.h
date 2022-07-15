// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HazeUtils.h"
#include "Subsystems/WorldSubsystem.h"
#include "HazeSubsystem.generated.h"

/**
 * 
 */
class UHazeComponent;

UCLASS()
class DEEPSEAHORROR_API UHazeSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

	ListenerUtils<HazeListener> m_Listeners;
	std::vector<UHazeComponent*> m_HazeSources;
public:
	void AddHazeListener(HazeListener* hazeListener);
	void RemoveHazeListener(HazeListener* hazeListener);
	
	void RegisterHazeSource(UHazeComponent* hazeComponent);
	void UnregisterHazeSource(UHazeComponent* hazeComponent);
	float PollHazeStrengthAtLocation(FVector worldLocation) const;
	void OnHazeSourceUpdated();
};
