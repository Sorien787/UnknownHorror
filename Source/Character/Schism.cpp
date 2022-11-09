// Fill out your copyright notice in the Description page of Project Settings.


#include "Schism.h"

#include "Kismet/KismetMathLibrary.h"

static TAutoConsoleVariable<int32> SchismDebug(
	TEXT("SchismDebug"),
	0,
	TEXT("Shows Schism debug visualization.\n"),
	ECVF_Scalability | ECVF_RenderThreadSafe);

FQuat ASchism::GetDesiredBodyRotation(float DeltaTime)
{
	FVector forwardVector = (m_pFrontRightLeg->GetCurrentFootPosition() - m_pBackRightLeg->GetCurrentFootPosition() + m_pFrontLeftLeg->GetCurrentFootPosition() - m_pBackLeftLeg->GetCurrentFootPosition())/2.0f;
	forwardVector = forwardVector.GetUnsafeNormal();
	
	FVector rightVector = (m_pFrontRightLeg->GetCurrentFootPosition() - m_pFrontLeftLeg->GetCurrentFootPosition() + m_pBackRightLeg->GetCurrentFootPosition() - m_pBackLeftLeg->GetCurrentFootPosition())/2.0f;
	rightVector = rightVector.GetUnsafeNormal();

	const FVector upVector = forwardVector.Cross(rightVector);
	
	rightVector = upVector.Cross(forwardVector);
	
	FQuat desiredQuat = UKismetMathLibrary::MakeRotationFromAxes(forwardVector,rightVector, upVector).Quaternion();
	
	const FQuat currentQuat = GetOwner()->GetActorRotation().Quaternion();

	// only change the relative yaw and the relative roll
	// relative pitch should stay the same

	const FQuat currentQuatToDesiredQuat = currentQuat.Inverse() * desiredQuat;

	FVector eulerRots = currentQuatToDesiredQuat.Euler();
	//
	// eulerRots.X *= m_LegAlignmentBodyRotationXSensitivity;
	// eulerRots.Y *= m_LegAlignmentBodyRotationYSensitivity;
	// eulerRots.Z *= m_LegAlignmentBodyRotationZSensitivity;
	//  roll X, pitch Y, yaw Z
	desiredQuat = currentQuat * FQuat::MakeFromEuler(eulerRots);

	if(SchismDebug.GetValueOnGameThread() > 0)
	{
		DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + desiredQuat.GetUpVector() * 100.0f, FColor::Green, false, -1, 0, 10);
		DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + desiredQuat.GetForwardVector() * 100.0f, FColor::Red, false, -1, 0, 10);
		DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + desiredQuat.GetRightVector() * 100.0f, FColor::Blue, false, -1, 0, 10);
	}
	
	return UKismetMathLibrary::QuaternionSpringInterp(currentQuat, desiredQuat,  m_RotSpringState, m_BodyRotationSpeed, m_BodyRotationDamping, DeltaTime);
}

bool ASchism::MakeLineTrace(FVector start, FVector end, FHitResult& hitResult)
{
	static FName TraceTag = TEXT("SchismTrace");
	FCollisionQueryParams traceParams(TraceTag, false, GetOwner());

	if (SchismDebug.GetValueOnGameThread() > 0)
		GetWorld()->DebugDrawTraceTag = TraceTag;
			
	return GetWorld()->LineTraceSingleByChannel(
		OUT hitResult,
		start,
		end,
		ECollisionChannel::ECC_WorldStatic,
		traceParams);
}

ASchism::AngleTestResult ASchism::DoAngleTest(FVector startA, FVector endA, FVector startB, FVector endB, float defaultLength, float angleCheckDistance, float sensitivity)
{
	FHitResult hit;
	float forwardLength = defaultLength;
	bool foundGeom = false;
	if (MakeLineTrace(startA, endA, hit))
	{
		foundGeom = true;
		forwardLength = hit.Distance;
	}
	float backwardLength = defaultLength;
	if (MakeLineTrace(startB, endB, hit))
	{
		foundGeom = true;
		backwardLength = hit.Distance;
	}
	
	// if forward > backward, tilt forward
	float resultant = forwardLength - backwardLength;
	// turn the resultant difference into a desired quaternion
	// can get desired angle tilt by tan
	float theta = -FMath::Atan(resultant / (2 * angleCheckDistance)) * sensitivity;
	theta = FMath::Clamp(theta, -m_MaxAngleOffsetFromCurrentForProceduralRotation, m_MaxAngleOffsetFromCurrentForProceduralRotation);
	
	return AngleTestResult(foundGeom, theta);
}

// Sets default values
ASchism::ASchism()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	m_pRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
	SetRootComponent(m_pRootComponent);

	m_pSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Skeletal Mesh"));
	check(m_pSkeletalMesh != nullptr);
	m_pSkeletalMesh->SetupAttachment(GetRootComponent());
	
	m_pLegManager = CreateDefaultSubobject<ULegManager>(TEXT("Leg Manager"));
	check(m_pLegManager != nullptr);
	m_pLegManager->SetupAttachment(GetRootComponent());

	m_pBoxComponentForLegManager = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Component"));
	check(m_pBoxComponentForLegManager != nullptr);
	m_pBoxComponentForLegManager->SetupAttachment(GetRootComponent());
	
	m_pHazeSource = CreateDefaultSubobject<UHazeComponent>(TEXT("Haze Component"));
	check(m_pHazeSource != nullptr);
	
	m_pBackLeftLeg = CreateDefaultSubobject<ULegComponent>(TEXT("Back Left Leg"));
	check(m_pBackLeftLeg != nullptr);
	m_pBackLeftLeg->SetupAttachment(GetRootComponent());
	
	m_pFrontLeftLeg = CreateDefaultSubobject<ULegComponent>(TEXT("Front Left Leg"));
	check(m_pFrontLeftLeg != nullptr);
	m_pFrontLeftLeg->SetupAttachment(GetRootComponent());
	
	m_pBackRightLeg = CreateDefaultSubobject<ULegComponent>(TEXT("Back Right Leg"));
	check(m_pBackRightLeg != nullptr);
	m_pBackRightLeg->SetupAttachment(GetRootComponent());

	m_pFrontRightLeg = CreateDefaultSubobject<ULegComponent>(TEXT("Front Right Leg"));
	check(m_pFrontRightLeg != nullptr);
	m_pFrontRightLeg->SetupAttachment(GetRootComponent());

	m_pLegManager->SetBoxForBodyCollision(m_pBoxComponentForLegManager);
}

// Called when the game starts or when spawned
void ASchism::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASchism::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// set the actor's rotation here
	//const FQuat desiredRotationByLegs = GetDesiredBodyRotation(DeltaTime);
	//
	// SetActorRotation(desiredRotation.Rotator(), ETeleportType::None);
	//
	//
	FQuat actorQuat = GetActorRotation().Quaternion();
	const FVector forwardBackwardPredictionVector = actorQuat.GetForwardVector() * m_ForwardPredictionLength; 
	const FVector leftRightPredictionVector = actorQuat.GetRightVector() * m_LeftRightPredictionLength;
	const FVector upDownPredictionVector = actorQuat.GetUpVector() * m_UpDownPredictionLength;
		
	const FVector forwardPoint = GetActorLocation() + forwardBackwardPredictionVector;
	const FVector leftPoint = GetActorLocation() - leftRightPredictionVector;
	const FVector rightPoint = GetActorLocation() + leftRightPredictionVector;
	const FVector rightUpPoint = leftPoint - upDownPredictionVector;
	const FVector rightDownPoint = rightPoint - upDownPredictionVector;
	const FVector forwardDownPoint = forwardPoint - upDownPredictionVector;
	const FVector forwardUpPoint = forwardPoint + upDownPredictionVector;
	const FVector backwardPoint = GetActorLocation() - forwardBackwardPredictionVector;
	const FVector backwardDownPoint = backwardPoint - upDownPredictionVector;
	const FVector backwardUpPoint = backwardPoint + upDownPredictionVector;

	AngleTestResult result = DoAngleTest(GetActorLocation(), forwardPoint, GetActorLocation(), backwardPoint, m_ForwardPredictionLength, m_ForwardPredictionLength, m_LegAlignmentBodyRotationPitchSensitivity);

	// add an additional pitch based on the above result (?)
	// pitch is y component
	FQuat additionalQuat = FQuat::MakeFromEuler(FVector(0.0f, result.desiredAngle, 0.0f));

	FQuat desiredQuat = actorQuat * additionalQuat;
		
	FQuat newActorQuat = UKismetMathLibrary::QuaternionSpringInterp(actorQuat, desiredQuat,  m_RotSpringState, m_BodyRotationSpeed, m_BodyRotationDamping, DeltaTime);
	// rotate until rightLength and leftLength are equal
	// so, essentially, want to measure the difference
	SetActorRotation(newActorQuat);
	// if there's uneven terrain ahead of us, leftLength and rightLength will capture that data
}

