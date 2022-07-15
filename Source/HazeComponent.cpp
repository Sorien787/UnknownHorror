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

float UHazeComponent::GetHazeFieldAtWorldLocation(FVector worldLocation) const
{
	FVector currentLocation = GetOwner()->GetActorLocation();
	float distSquared = FVector::DistSquared(currentLocation, worldLocation);
	// we can assume that no haze component will have a haze effect component added on
	// so, distSquared is never zero.
	// just in case though, let's guard for it.
	if (distSquared < FLT_EPSILON)
	{
		return FLT_MAX;		
	}
	return m_HazeStrength / distSquared;
}


// Called when the game starts
void UHazeComponent::BeginPlay()
{
	Super::BeginPlay();
	UWorld* world = GetWorld();

	if (!world)
		return;
	
	UHazeSubsystem* hazeSubsystem = world->GetSubsystem<UHazeSubsystem>();

	if (!hazeSubsystem)
		return;
	
	hazeSubsystem->RegisterHazeSource(this);
}

void UHazeComponent::BeginDestroy()
{
	Super::BeginDestroy();
	UWorld* world = GetWorld();

	if (!world)
		return;
	
	UHazeSubsystem* hazeSubsystem = world->GetSubsystem<UHazeSubsystem>();
	
	if (!hazeSubsystem)
		return;
	
	hazeSubsystem->UnregisterHazeSource(this);
}



// Called every frame
void UHazeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

