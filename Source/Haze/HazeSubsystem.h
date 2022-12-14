// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <thread>
#include <condition_variable>
#include <mutex>

#include "CoreMinimal.h"
#include "HazeGridActor.h"
#include "Subsystems/WorldSubsystem.h"
#include "HazeSubsystem.generated.h"

/**
 * 
 */
class UHazeComponent;


UCLASS()
class DEEPSEAHORROR_API UHazeSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

	std::vector<AHazeGridActor*> m_HazeGrids;

	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;
	void ThreadUpdateLoop(float deltaTime);

	bool HasHazeUpdateFinished()const;

	bool m_bHasHazeUpdateFinished{ true };

	std::mutex m_Mutex;
	std::thread m_UpdateLoopThread;
	std::condition_variable m_UpdateLoopConditionVariable;

	float m_OverrideHazeDiffusionCoefficient{-1.0f};
	float m_OverrideHazeDrainageCoefficient{-1.0f};
public:
	virtual ~UHazeSubsystem() override;
	
	void RegisterHazeGrid(AHazeGridActor* hazeGrid);
	void UnregisterHazeGrid(AHazeGridActor* hazeGrid);
	
	float PollHazeStrengthAtLocation(FVector strengthAtLocation, int hazeGridID) const;
	void AddHazeSourceAtLocation(FVector strengthAtLocation, float strength, int hazeGridID);

	UFUNCTION(BlueprintCallable)
	void OverrideHazeGridsDiffusionValue(float diffusionValue);

	UFUNCTION(BlueprintCallable)
	void OverrideHazeGridsDrainageValue(float drainageValue);

	int GetHazeIDAtLocation(FVector newLocation, int hazeGridID) const;
};
