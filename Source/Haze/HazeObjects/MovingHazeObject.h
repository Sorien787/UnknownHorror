// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../HazeEffectComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/SphereComponent.h"
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

	FVector TryGetNewObjectPlacementPoint(float objectSize, int objectIndex);
	
	std::vector<ObjectData> m_Objects;
	
public:	
	AMovingHazeObject();

	void AddObject(UStaticMeshComponent* pMesh);

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	float m_TimeOffScreenBeforeCanMove;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* m_pObjectMesh;
	
	UPROPERTY(EditAnywhere)
	UHazeEffectComponent* m_pHazeEffector;

	UPROPERTY(EditAnywhere)
	FRuntimeFloatCurve m_HazeStrengthToShiftProbabilityPerSecond;

	UPROPERTY(EditAnywhere)
	USphereComponent* m_pPlacementArea;

	UPROPERTY(EditAnywhere)
	UArrowComponent* m_pPlacementDirection;

};
