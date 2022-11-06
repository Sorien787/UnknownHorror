// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../../Gameplay/LightModifierComponent.h"
#include "GameFramework/Actor.h"
#include "../HazeUtils.h"
#include "../HazeEffectComponent.h"
#include "LightHazeActor.generated.h"

UCLASS()
class DEEPSEAHORROR_API ALightHazeActor : public AActor, public HazeComponentListener
{
	GENERATED_BODY()


public:	
	// Sets default values for this actor's properties
	ALightHazeActor();

	virtual void BeginPlay() override;

	virtual void OnHazeEvent() override;

	virtual void OnHazeFinish() override;

	virtual void OnHazeStart() override;

	virtual void Tick(float DeltaSeconds) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	ULightModifierComponent* m_pLightModifier;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UHazeEffectComponent* m_pHazeEffectComponent;

	UPROPERTY(EditAnywhere, Category = Setup)
	FRuntimeFloatCurve m_LightPercentOnlineByHazeModifier;

	UPROPERTY(EditAnywhere, Category = Setup)
	FRuntimeFloatCurve m_LightFlickerFrequencyByHazeModifier;

	UPROPERTY(EditAnywhere, Category = Setup)
	FRuntimeFloatCurve m_LightMinBrightness;

	UPROPERTY(EditAnywhere, Category = Setup)
	FRuntimeFloatCurve m_LightMaxBrightness;
};
