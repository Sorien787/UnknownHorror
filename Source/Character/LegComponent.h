// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "LegComponent.generated.h"

class ULegManager;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DEEPSEAHORROR_API ULegComponent : public USceneComponent
{
	GENERATED_BODY()

	bool m_bIsPlanted{true};

	bool m_bCanUnplant{false};

	float m_fUnplantStartTime{0.0f};

	float m_fPlantStartTime{0.0f};

	bool m_bHasMovedEnoughToPlant{true};


public:	
	// Sets default values for this component's properties
	ULegComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	

	UPROPERTY(EditAnywhere)
	FRuntimeFloatCurve m_DistanceByTime;

	UPROPERTY(EditAnywhere)
	FRuntimeFloatCurve m_HeightByTime;

	UPROPERTY(BlueprintReadOnly)
	FVector m_TracePosition;

	UPROPERTY(BlueprintReadOnly)
	FVector m_StartFootPosition;
	
	UPROPERTY(BlueprintReadOnly)
	FVector m_CurrentFootPosition;

	UPROPERTY(BlueprintReadOnly)
	FVector m_EndFootPosition;

	FVector GetLegUpVector() const;
	
	void RenderDebugInfo() const;

	float GetTimeSinceLastPlant() const;

	float GetPlantedQuantity(float stepTime) const;

	bool VerifyCanUnplant(float stepDistance);

	void UnplantFoot();

	void SetNewFootTargetLocation(float rayCastLength);

	void UpdateFootState(float stepTime, float totalStepHeight);

};
