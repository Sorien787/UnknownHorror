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
	
public:
	
	FEnterLightAreaDelegate m_EnterLightAreaDelegate;
	
	void OnEnterLightArea();

	FExitLightAreaDelegate m_ExitLightAreaDelegate;

	void OnExitLightArea();
};
