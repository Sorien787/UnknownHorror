// Fill out your copyright notice in the Description page of Project Settings.


#include "UnrealUtilities.h"

UnrealUtilities::UnrealUtilities()
{
}

UnrealUtilities::~UnrealUtilities()
{
}

FVector UnrealUtilities::RaycastActorToWorldPosition(const UWorld* world, const float range, const AActor* pIgnoreActor)
{
	FVector playerViewPointLocation;
	FRotator playerRotationInformation;
	
	world->GetFirstPlayerController()->GetPlayerViewPoint(OUT playerViewPointLocation, OUT playerRotationInformation);
	
	FVector lineTraceEnd = playerViewPointLocation + playerRotationInformation.Vector() * range;

	FHitResult hit;
	// false to ignore complex collisions
	FCollisionQueryParams traceParams(FName(TEXT("Raycast To Interactable")), false, pIgnoreActor);

	world->LineTraceSingleByObjectType(
		OUT hit,
		playerViewPointLocation,
		lineTraceEnd,
		FCollisionObjectQueryParams(ECollisionChannel::ECC_Visibility),
		traceParams);

	AActor* pActorHit = hit.GetActor();
	
	if (!pActorHit)
		return lineTraceEnd;
	
	return  hit.Location;
}

FHitResult UnrealUtilities::RaycastActorToWorldHit(const UWorld* world, const float range, const AActor* pIgnoreActor)
{
	FVector playerViewPointLocation;
	FRotator playerRotationInformation;
	
	world->GetFirstPlayerController()->GetPlayerViewPoint(OUT playerViewPointLocation, OUT playerRotationInformation);
	
	FVector lineTraceEnd = playerViewPointLocation + playerRotationInformation.Vector() * range;

	FHitResult hit;
	// false to ignore complex collisions
	FCollisionQueryParams traceParams(FName(TEXT("Raycast To Interactable")), false, pIgnoreActor);

	world->LineTraceSingleByObjectType(
		OUT hit,
		playerViewPointLocation,
		lineTraceEnd,
		FCollisionObjectQueryParams(ECollisionChannel::ECC_Visibility),
		traceParams);
	
	return hit;
}

