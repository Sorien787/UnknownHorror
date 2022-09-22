// Fill out your copyright notice in the Description page of Project Settings.


#include "ProximityInteraction.h"

// Sets default values
AProximityInteraction::AProximityInteraction()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

bool AProximityInteraction::GetActorTriggerConditionMet(FTransform actorTransform, FVector actorVelocity) const
{
	const FTransform& interactionPointActorTransform = GetActorTransform();
	const FVector interactionPointForwardVector = interactionPointActorTransform.GetUnitAxis(EAxis::X);
	const FVector interactionPointPosition = interactionPointActorTransform.GetLocation();
	
	FVector interactionPoint_to_otherActor = actorTransform.GetLocation() - interactionPointPosition;
	
	if (m_RequireMovementInDirection && !actorVelocity.Normalize())
		return false;

	const float angleBetweenVelocityAndActorOffset = FMath::Acos(FVector::DotProduct(interactionPoint_to_otherActor, interactionPointForwardVector));

	if (m_RequireMovementInDirection && angleBetweenVelocityAndActorOffset > 90.0f)
		return false;
	
	if (!interactionPoint_to_otherActor.Normalize())
		return true;

	const float angleBetweenForwardAndActorOffset = FMath::Acos(FVector::DotProduct(interactionPoint_to_otherActor, interactionPointForwardVector));

	return FMath::RadiansToDegrees(angleBetweenForwardAndActorOffset) < m_TriggerAngle;
}

// Called when the game starts or when spawned
void AProximityInteraction::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AProximityInteraction::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

