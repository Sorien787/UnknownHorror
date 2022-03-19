// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class DEEPSEAHORROR_API UnrealUtilities
{
public:
	static FVector RaycastActorToWorldPosition(const UWorld* world, const float range, const AActor* pIgnoreActor);
	static FHitResult RaycastActorToWorldHit(UWorld* world, const float range, const AActor* pIgnoreActor);
	static FRotator GetRotationMatrixToPlayer(const UWorld* world, const FVector objectLocation);
	static FTransform GetPlayerCameraTransform(const UWorld* world);
	UnrealUtilities();
	~UnrealUtilities();
};
