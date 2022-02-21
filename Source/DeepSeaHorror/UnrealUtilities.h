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
	static FHitResult RaycastActorToWorldHit(const UWorld* world, const float range, const AActor* pIgnoreActor);
	UnrealUtilities();
	~UnrealUtilities();
};
