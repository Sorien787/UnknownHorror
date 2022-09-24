// Fill out your copyright notice in the Description page of Project Settings.


#include "UnrealUtilities.h"

UnrealUtilities::UnrealUtilities()
{
}

UnrealUtilities::~UnrealUtilities()
{
}

bool UnrealUtilities::IsInFrustrum( FVector location, float size, UWorld* pWorld)
{
	ULocalPlayer* LocalPlayer = pWorld->GetFirstLocalPlayerFromController();
	
	if (LocalPlayer == nullptr || LocalPlayer->ViewportClient == nullptr || !LocalPlayer->ViewportClient->Viewport)
		return false;
	
	FSceneViewFamilyContext ViewFamily(FSceneViewFamily::ConstructionValues(
		LocalPlayer->ViewportClient->Viewport,
		pWorld->Scene,
		LocalPlayer->ViewportClient->EngineShowFlags)
		.SetRealtimeUpdate(true));

	FVector ViewLocation;
	FRotator ViewRotation;
	FSceneView* SceneView = LocalPlayer->CalcSceneView(&ViewFamily, ViewLocation, ViewRotation, LocalPlayer->ViewportClient->Viewport);
	
	if (SceneView == nullptr)
		return false;
	
	return SceneView->ViewFrustum.IntersectSphere(location, size);
}

float UnrealUtilities::GetRadAngleBetweenVectors(FVector a, FVector b)
{
	return FMath::Acos(FVector::DotProduct(a.GetUnsafeNormal(), b.GetUnsafeNormal()));
}

FVector UnrealUtilities::GetArbitraryNormalVector(FVector basisA)
{
	FVector someDirection = FVector(1, 0, 0);
	
	if(basisA.Dot(someDirection) > 1 - FLT_EPSILON)
		someDirection = FVector(0, 1, 0);

	FVector basisB = someDirection - someDirection.Dot(basisA) * basisA;
	basisB.Normalize();
	
	return basisB;
}

FVector UnrealUtilities::RaycastActorToWorldPosition(const UWorld* world, const float range, const AActor* pIgnoreActor)
{
	FVector playerViewPointLocation;
	FRotator playerRotationInformation;
	
	world->GetFirstPlayerController()->GetPlayerViewPoint(OUT playerViewPointLocation, OUT playerRotationInformation);
	
	FVector lineTraceEnd = playerViewPointLocation + playerRotationInformation.Vector() * range;

	FHitResult hit;
	// false to ignore complex collisions
	FCollisionQueryParams traceParams(FName(TEXT("Raycast To Interactable")), true, pIgnoreActor);

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

TArray<FHitResult> UnrealUtilities::RaycastActorToWorldHit(UWorld* world, const float range, const AActor* pIgnoreActor)
{
	FVector playerViewPointLocation;
	FRotator playerRotationInformation;
	
	world->GetFirstPlayerController()->GetPlayerViewPoint(OUT playerViewPointLocation, OUT playerRotationInformation);
	
	FVector lineTraceEnd = playerViewPointLocation + playerRotationInformation.Vector() * range;

	FHitResult hit;
	// false to ignore complex collisions

	static FName TraceTag = TEXT("TraceTag");
	FCollisionQueryParams traceParams(TraceTag, false, pIgnoreActor);
	TArray<FHitResult> outHits;
	world->LineTraceMultiByObjectType(
		OUT outHits,
		playerViewPointLocation,
		lineTraceEnd,
		FCollisionObjectQueryParams(ECollisionChannel::ECC_WorldStatic | ECollisionChannel::ECC_WorldDynamic),
		traceParams);
	
	return outHits;
}

FRotator UnrealUtilities::GetRotationMatrixToPlayer(const UWorld* world, const FVector objectLocation)
{
	FVector cameraLocation = world->GetFirstPlayerController()->PlayerCameraManager->GetCameraLocation();

	return (cameraLocation - objectLocation).Rotation();
}

FTransform UnrealUtilities::GetPlayerCameraTransform(const UWorld* world)
{
	return world->GetFirstPlayerController()->PlayerCameraManager->GetTransform();
}


