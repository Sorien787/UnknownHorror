// Fill out your copyright notice in the Description page of Project Settings.


#include "HazeComponent.h"

#include "HazeSubsystem.h"

// Sets default values for this component's properties
UHazeComponent::UHazeComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UHazeComponent::BeginPlay()
{
	Super::BeginPlay();
	UWorld* world = GetWorld();
	UHazeSubsystem* hazeSubsystem = world->GetSubsystem<UHazeSubsystem>();
	hazeSubsystem->RegisterHazeSource(this);
}

void UHazeComponent::BeginDestroy()
{
	Super::BeginDestroy();
	UWorld* world = GetWorld();
	UHazeSubsystem* hazeSubsystem = world->GetSubsystem<UHazeSubsystem>();
	hazeSubsystem->UnregisterHazeSource(this);
}



// Called every frame
void UHazeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

