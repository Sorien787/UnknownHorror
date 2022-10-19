// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LightSensitivityComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FEnterLightAreaDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FExitLightAreaDelegate);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DEEPSEAHORROR_API ULightSensitivityComponent : public UActorComponent
{
	GENERATED_BODY()
	
	int numLightsInside = 0;

public:

	bool IsCurrentlyLit() const;
	
	FEnterLightAreaDelegate m_EnterLightAreaDelegate;

	UFUNCTION()
	void OnEnterLightArea();

	FExitLightAreaDelegate m_ExitLightAreaDelegate;
	
	UFUNCTION()
	void OnExitLightArea();
};
