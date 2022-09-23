// Fill out your copyright notice in the Description page of Project Settings.


#include "Haze/HazeObjects/MovingHazeObject.h"

FVector AMovingHazeObject::TryGetNewObjectPlacementPoint(float objectSize, int objectIndex)
{
	FVector2d circlePoint = FMath::RandPointInCircle(m_pPlacementArea->Bounds.SphereRadius);
	FVector circleNorm = m_pPlacementDirection->GetForwardVector();
	FVector axisA = UnrealUtilities::GetArbitraryNormalVector(circleNorm);
	FVector axisB = circleNorm.Cross(axisA);
	FVector total = axisA * circlePoint.X + axisB * circlePoint.Y;
	FVector pointOnCircle = circleNorm * m_pPlacementArea->Bounds.SphereRadius;
	FVector lineTraceStart = GetActorLocation() + pointOnCircle + total;
	FVector lineTraceEnd = GetActorLocation() - pointOnCircle + total;

	static FName TraceTag = TEXT("TraceTag");
	FCollisionQueryParams traceParams(TraceTag, false);
	FHitResult outHit;
	bool hasHit = GetWorld()->LineTraceSingleByObjectType(
		OUT outHit,
		lineTraceStart,
		lineTraceEnd,
		FCollisionObjectQueryParams(ECollisionChannel::ECC_WorldStatic | ECollisionChannel::ECC_WorldDynamic),
		traceParams);
	
	if (!hasHit)
		return m_Objects[objectIndex].pObject->GetComponentLocation();

	for (int i = 0; i < m_Objects.size(); i++)
	{
		if (i == objectIndex)
			continue;
		
		ObjectData& obj = m_Objects[i];
		
		FVector objectLocation = obj.pObject->GetComponentLocation();

		float objectDoubleRadii = obj.nObjectSize + m_Objects[objectIndex].nObjectSize;
		
		if (FVector::DistSquared(objectLocation, outHit.Location) < objectDoubleRadii * objectDoubleRadii)
			return m_Objects[objectIndex].pObject->GetComponentLocation();
	}
	
	return outHit.Location;
}

// Sets default values
AMovingHazeObject::AMovingHazeObject()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void AMovingHazeObject::AddObject(UStaticMeshComponent* pMesh)
{
	FVector min;
	FVector max;
	pMesh->GetLocalBounds(min, max);
	const FVector extent = max - min;
	m_Objects.emplace_back(pMesh, false, extent.Length());
}

// Called when the game starts or when spawned
void AMovingHazeObject::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMovingHazeObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!m_pHazeEffector->IsHazeActive())
		return;

	if (m_pPlacementArea->WasRecentlyRendered(m_TimeOffScreenBeforeCanMove))
		return;
	
	for (int i = 0; i < m_Objects.size(); i++)
	{
		ObjectData& obj = m_Objects[i];
		


		if (obj.bHasMoved)
			continue;
		
		const float probabilityToShiftPerSecond = m_HazeStrengthToShiftProbabilityPerSecond.EditorCurveData.Eval(m_pHazeEffector->GetCurrentHazeStrength());

		if (DeltaTime * probabilityToShiftPerSecond < FMath::RandRange(0.0f, 1.0f))
			continue;

		FVector newPoint = TryGetNewObjectPlacementPoint(obj.nObjectSize, i);

		if (FVector::DistSquared(newPoint, obj.pObject->GetComponentLocation()) < FLT_EPSILON)
			continue;

		obj.pObject->SetWorldLocation(newPoint);
		obj.bHasMoved = true;
	}

}

