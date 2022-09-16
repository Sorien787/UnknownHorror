// Fill out your copyright notice in the Description page of Project Settings.


#include "LegManager.h"

// Sets default values for this component's properties
ULegManager::ULegManager()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void ULegManager::BeginPlay()
{
	Super::BeginPlay();
	
	GetOwner()->GetComponents<ULegComponent>(m_controllableLegComponents);

	// ...
	
}


// Called every frame
void ULegManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	ULegComponent* bestLeg = nullptr;

	float totalLegUnplantQuantity = 0.0f;

	for (const auto& leg : m_controllableLegComponents)
	{
		leg->UpdateFootState(StepTime, StepHeight);
		leg->SetNewFootTargetLocation(RayCastLength);
		totalLegUnplantQuantity += leg->GetTimeSinceLastPlant();

		// if leg internally can't unplant, early out
		if (!leg->VerifyCanUnplant(StepDistance))
			continue;

		// or if leg, due to other legs being better candidates, cant unplant, early out
		if (bestLeg && (bestLeg->GetTimeSinceLastPlant() > leg->GetTimeSinceLastPlant()))
			continue;
		
		bestLeg = leg;
	}

	// if leg state means no new leg can unplant, early out
	if (totalLegUnplantQuantity < LegUnplantThreshold)
		return;

	if (!bestLeg)
		return;

	bestLeg->UnplantFoot();
}

