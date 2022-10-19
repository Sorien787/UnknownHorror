// Fill out your copyright notice in the Description page of Project Settings.


#include "Haze/HazeObjects/MovingHazeObject.h"

static TAutoConsoleVariable<int32> CVarMovingHazeObjectDebug(
	TEXT("MovingHazeObjectDebug"),
	0,
	TEXT("Shows visualization of where objects can move to, and what their sizes are.\n"),
	ECVF_Scalability | ECVF_RenderThreadSafe);



bool AMovingHazeObject::TryGetNewObjectPlacementPoint(FVector& position, FVector& normal, float objectSize, int objectIndex)
{
	float circleRadius = m_pPlacementArea->Bounds.SphereRadius - objectSize;
	FVector2d circlePoint = FMath::RandPointInCircle(circleRadius);
	FVector circleNorm = m_pPlacementDirection->GetForwardVector();
	FVector axisA = UnrealUtilities::GetArbitraryNormalVector(circleNorm);
	FVector axisB = circleNorm.Cross(axisA);
	FVector total = axisA * circlePoint.X + axisB * circlePoint.Y;
	FVector pointOnCircle = circleNorm * m_pPlacementArea->Bounds.SphereRadius;
	FVector lineTraceStart = GetActorLocation() + pointOnCircle + total;
	FVector lineTraceEnd = GetActorLocation() - pointOnCircle + total;

	static FName TraceTag = TEXT("MovingHazeTrace");
	if (CVarMovingHazeObjectDebug->GetInt())
	{
		GetWorld()->DebugDrawTraceTag = TraceTag;
	}
	FCollisionQueryParams traceParams(TraceTag, false);
	FHitResult outHit;
	bool hasHit = GetWorld()->LineTraceSingleByObjectType(
		OUT outHit,
		lineTraceStart,
		lineTraceEnd,
		FCollisionObjectQueryParams(ECollisionChannel::ECC_WorldStatic),
		traceParams);
	
	if (!hasHit)
		return false;

	for (int i = 0; i < m_Objects.size(); i++)
	{
		if (i == objectIndex)
			continue;
		
		ObjectData& obj = m_Objects[i];
		
		FVector objectLocation = obj.pObject->GetComponentLocation();

		float objectDoubleRadii = obj.nObjectSize + m_Objects[objectIndex].nObjectSize;
		
		if (FVector::DistSquared(objectLocation, outHit.Location) < objectDoubleRadii * objectDoubleRadii)
			return false;
	}

	position = outHit.Location;
	normal = outHit.Normal;
	return true;
}

// Sets default values
AMovingHazeObject::AMovingHazeObject()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	m_pPlacementArea = CreateDefaultSubobject<USphereComponent>(TEXT("Area Component"));
	check(m_pPlacementArea != nullptr);
	//
	m_pPlacementDirection = CreateDefaultSubobject<UArrowComponent>(TEXT("Placement Component"));
	check(m_pPlacementDirection != nullptr);

	m_pHazeEffector = CreateDefaultSubobject<UHazeEffectComponent>(TEXT("Haze Component"));
	check(m_pHazeEffector != nullptr);

	m_pLightSensitivityComponent = CreateDefaultSubobject<ULightSensitivityComponent>(TEXT("Light Sensitivity Component"));
	check(m_pLightSensitivityComponent != nullptr);
}

void AMovingHazeObject::AddObject(UStaticMeshComponent* pMesh)
{
	FVector min;
	FVector max;
	pMesh->GetLocalBounds(min, max);
	const FVector extent = max - min;
	m_Objects.emplace_back(pMesh, false, extent.Length());
}

void AMovingHazeObject::MoveObject(ObjectData& objData, int i)
{
	FVector position;
	FVector normal;
	const bool foundNewPoint = TryGetNewObjectPlacementPoint(position, normal, objData.nObjectSize, i);

	normal = -normal;
		
	if (!foundNewPoint)
		return;

	objData.pObject->SetWorldLocation(position);
	objData.bHasMoved = true;
	m_bHasAnyObjectMoved = true;

	if (!m_bRandomizeRotation)
		return;

	FVector upVec = FVector(0, 0, 1);
	FVector rotateAround = normal.Cross(upVec).GetUnsafeNormal();

	float tilt = UnrealUtilities::GetRadAngleBetweenVectors(normal, upVec);

	const float randAngle = FMath::FRandRange(0.0f, 360.0f);
		
	const FQuat rotator = FQuat(rotateAround, -tilt) *  FQuat(upVec,  randAngle);

	objData.pObject->SetWorldRotation(rotator);
}

// Called when the game starts or when spawned
void AMovingHazeObject::BeginPlay()
{
	Super::BeginPlay();

	TArray<UStaticMeshComponent*> meshComponents;
	GetComponents<UStaticMeshComponent>(meshComponents);
	for (int i = 0; i < meshComponents.Num(); i++)
	{
		AddObject(meshComponents[i]);
	}
}

// Called every frame
void AMovingHazeObject::Tick(float DeltaTime)
{
	if (CVarMovingHazeObjectDebug->GetInt() > 0)
	{
		DrawDebugSphere(GetWorld(), m_pPlacementArea->GetComponentLocation(), m_pPlacementArea->Bounds.SphereRadius, 32, FColor::Black);

		for (int i = 0; i < m_Objects.size(); i++)
		{
			DrawDebugSphere(GetWorld(), m_Objects[i].pObject->GetComponentLocation(), m_Objects[i].nObjectSize, 32, FColor::Green);
			
		}
	}
	
	Super::Tick(DeltaTime);

	if (!m_pHazeEffector->IsHazeActive())
		return;

	const bool isInLight = m_pLightSensitivityComponent->IsCurrentlyLit();
	const bool isInFrustrum = UnrealUtilities::IsInFrustrum(GetActorLocation(), m_pPlacementArea->Bounds.SphereRadius, GetWorld());
	const bool isUnobserved = (!isInFrustrum || !isInLight);

	if (!isUnobserved)
	{
		m_TimeLastInFrustrum = GetWorld()->GetTimeSeconds();

		if (!m_bHasAnyObjectMoved)
			return;
		
		for (int i = 0; i < m_Objects.size(); i++)
		{
			m_Objects[i].bHasMoved = false;
		}
		
		m_bHasAnyObjectMoved = false;
		return;
	}

	const float timeBetweenNextShift = m_HazeStrengthToTimeBetweenShifts.EditorCurveData.Eval(m_pHazeEffector->GetCurrentHazeStrength());
	if (GetWorld()->GetTimeSeconds() - m_TimeLastInFrustrum < timeBetweenNextShift)
		return;


	for (int i = 0; i < m_Objects.size(); i++)
	{
		ObjectData& obj = m_Objects[i];
		
		if (obj.bHasMoved)
			continue;
		
		const float probabilityToShiftPerSecond = m_HazeStrengthToShiftProbabilityPerSecond.EditorCurveData.Eval(m_pHazeEffector->GetCurrentHazeStrength());

		if (DeltaTime * probabilityToShiftPerSecond < FMath::RandRange(0.0f, 1.0f))
			continue;

		MoveObject(obj, i);
	}

}

