// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../HazeEffectComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/SphereComponent.h"
#include "Lighting/LightSensitivityComponent.h"
#include "MovingHazeObject.generated.h"

struct ObjectData
{
	ObjectData(UStaticMeshComponent* _pObject, bool _bHasMoved, float _nObjectSize) : pObject(_pObject), bHasMoved(_bHasMoved), nObjectSize(_nObjectSize){}
	UStaticMeshComponent* pObject{nullptr};
	bool bHasMoved{false};
	float nObjectSize{0.0f};
};

UCLASS()
class DEEPSEAHORROR_API AMovingHazeObject : public AActor
{
	GENERATED_BODY()

	bool TryGetNewObjectPlacementPoint(FVector& position, FVector& normal, float objectSize, int objectIndex);
	
	std::vector<ObjectData> m_Objects;

	float m_TimeLastInFrustrum = 0.0f;

	bool m_bHasAnyObjectMoved = false;
	
public:	
	AMovingHazeObject();

	void AddObject(UStaticMeshComponent* pMesh);

	void MoveObject(ObjectData& objData, int i);

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(EditAnywhere)
	FRuntimeFloatCurve m_HazeStrengthToShiftProbabilityPerSecond;

	UPROPERTY(EditAnywhere)
	FRuntimeFloatCurve m_HazeStrengthToTimeBetweenShifts;

	UPROPERTY(EditAnywhere)
	FRuntimeFloatCurve m_HazeStrengthToDistanceShifted;

	UPROPERTY(EditAnywhere)
	bool m_bRandomizeRotation{false};
	
	UPROPERTY(EditAnywhere)
	USphereComponent* m_pPlacementArea;

	UPROPERTY(EditAnywhere)
	UArrowComponent* m_pPlacementDirection;

	UPROPERTY(EditAnywhere)
	UHazeEffectComponent* m_pHazeEffector;

	UPROPERTY(EditAnywhere)
	ULightSensitivityComponent* m_pLightSensitivityComponent;
};
