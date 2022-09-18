// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ListenerUtils.h"
#include "HazeUtils.h"
#include "HazeComponent.h"
#include "GameFramework/Actor.h"
#include "HazeGridActor.generated.h"


struct PendingChange
{
	PendingChange(int _gridIndex, float _value) : gridIndex(_gridIndex), value(_value) {}
	int gridIndex;
	float value;
};

UCLASS()
class DEEPSEAHORROR_API AHazeGridActor : public AActor
{
	GENERATED_BODY()
	
	ListenerUtils<HazeListener> m_Listeners;
	std::vector<UHazeComponent*> m_HazeSources;
	DiffusionGrid m_HazeGrid;
	DiffusionGrid m_WorkingHazeGrid;
	std::vector<PendingChange> m_PendingChanges;
	
public:	
	// Sets default values for this actor's properties
	AHazeGridActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called when a property is edited
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	
	virtual void BeginDestroy() override;

public:	
	bool GridContainsPoint(FVector point) const;

	float SampleGrid(FVector point) const;

	void SyncChangesToWorkingGrid();

	void UpdateDiffusion(float deltaTime);

	void AddSampleToGrid(FVector point, float sample);

	UPROPERTY(EditAnywhere)
	FIntVector gridExtents;

	UPROPERTY(EditAnywhere)
	float gridElementSize;

	UPROPERTY(EditAnywhere)
	float diffusionCoefficient;

	UPROPERTY(EditAnywhere)
	float drainageCoefficient;

	UPROPERTY(EditAnywhere)
	float valueForMaxDebug;

};
