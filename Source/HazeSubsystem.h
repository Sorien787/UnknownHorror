// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HazeUtils.h"
#include "HazeGridActor.h"
#include "array"
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


	std::vector<AHazeGridActor*> m_HazeGrids;

	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	
public:
	void RegisterHazeGrid(AHazeGridActor* hazeGrid);
	void UnregisterHazeGrid(AHazeGridActor* hazeGrid);
	
	float PollHazeStrengthAtLocation(FVector strengthAtLocation, int hazeGridID) const;
	void AddHazeSourceAtLocation(FVector strengthAtLocation, float strength, int hazeGridID);

	int GetHazeIDAtLocation(FVector newLocation, int hazeGridID) const;
};
