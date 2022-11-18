// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <deque>

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "LegComponent.generated.h"

class ULegManager;

USTRUCT(BlueprintType)
struct FLegRaycastProfile
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float raycastSize{0.0f};

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float raycastLength{0.0f};

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float lateralDistance{0.0f};

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float angleTowardMovementDir{0.0f};
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DEEPSEAHORROR_API ULegComponent : public USceneComponent
{
	GENERATED_BODY()

	bool m_bIsFootPlanted{true};
	
	bool m_bCanUnplant{false};

	float m_fUnplantStartTime{0.0f};

	float m_fPlantStartTime{0.0f};
	FVector m_stepUpVector{FVector(0, 1, 0)};
	float m_timeTakenForStep{0.0f};
	float m_lastStepHeight{0.0f};
	float m_additionalStepHeight{0.0f};
	
	std::deque<FVector> m_historicalPathTracePoints;

public:	
	// Sets default values for this component's properties
	ULegComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
public:	

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool m_bReverseRaycastDirection{false};
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FRuntimeFloatCurve m_DistanceByTime;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FRuntimeFloatCurve m_HeightByTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int m_LegGroup;
	
	UPROPERTY(BlueprintReadOnly)
	FVector m_TracePosition;

	UPROPERTY(BlueprintReadOnly)
	FVector m_StartFootPosition;
	
	UPROPERTY(BlueprintReadOnly)
	FVector m_CurrentFootPosition;

	UPROPERTY(BlueprintReadOnly)
	FVector m_EndFootPosition;

	void UnplantFoot();
	bool TrySetNewFootTargetLocation(const TArray<FLegRaycastProfile>& raycastProfile, const FTransform& creatureTransform, const FVector& creatureVelocity,float stepTime, float stepHeight, float velocityStepDistance);

	float GetSqDistanceFromStartPoint() const;
	bool GetIsPlanted() const;
	int GetLegGrouping() const;
	void RenderDebugInfo() const;
	float GetFootHeightDefaultRelative(FVector upVector) const;
	float GetTimeSinceLastPlant() const;
	FVector GetCurrentFootPosition() const;
};

