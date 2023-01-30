// Fill out your copyright notice in the Description page of Project Settings.


#include "Schism.h"

#include "Common/UnrealUtilities.h"
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
	return desiredQuat;
	// return UKismetMathLibrary::QuaternionSpringInterp(currentQuat, desiredQuat,  m_RotSpringState, m_BodyRotationSpeed, m_BodyRotationDamping, DeltaTime);
}

bool ASchism::MakeLineTrace(FVector start, FVector end, FHitResult& hitResult)
{
	static FName TraceTag = TEXT("SchismTrace");
	FCollisionQueryParams traceParams(TraceTag, false, GetOwner());


	bool hit = GetWorld()->LineTraceSingleByChannel(
		OUT hitResult,
		start,
		end,
		ECollisionChannel::ECC_WorldStatic,
		traceParams);

	
	if (SchismDebug.GetValueOnGameThread() > 0)
	{
		UnrealUtilities::DrawLineTrace(GetWorld(), hit, start, end, hitResult);
	}
	return hit;
}

ASchism::AngleTestResult ASchism::DoAngleTest(FVector startA, FVector endA, FVector startB, FVector endB, float defaultLength, float angleCheckDistanceStart, float angleCheckDistanceFinish)
{
	FHitResult hit;
	float forwardLength = angleCheckDistanceFinish;
	bool foundGeom = false;
	if (MakeLineTrace(startA, endA, hit))
	{
		foundGeom = true;
		forwardLength = hit.Distance;
	}
	forwardLength -= angleCheckDistanceStart;
	float backwardLength = angleCheckDistanceFinish;
	if (MakeLineTrace(startB, endB, hit))
	{
		foundGeom = true;
		backwardLength = hit.Distance;
	}
	backwardLength -= angleCheckDistanceStart;
	
	// if forward > backward, tilt forward
	float lengthDifference = angleCheckDistanceFinish - angleCheckDistanceStart;
	float resultant = FMath::Clamp(forwardLength - backwardLength, -lengthDifference, lengthDifference);
	// turn the resultant difference into a desired quaternion
	// can get desired angle tilt by tan
	float theta = FMath::Atan(resultant / (2 * defaultLength));
	float maxTheta = FMath::Atan(lengthDifference / (2 * defaultLength));
	theta = theta / maxTheta;

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

	m_pMovementComponent = CreateDefaultSubobject<UWallClimbingMovementComponent>(TEXT("Movement Component"));
	check(m_pMovementComponent != nullptr);

	m_pLegManager->SetMovementComponent(m_pMovementComponent);
	m_pLegManager->SetBoxForBodyCollision(m_pBoxComponentForLegManager);
}

// Called when the game starts or when spawned
void ASchism::BeginPlay()
{
	Super::BeginPlay();
	
}

bool ASchism::CanBeSeenFrom(const FVector& ObserverLocation, FVector& OutSeenLocation, int32& NumberOfLoSChecksPerformed, float& OutSightStrength, const AActor* IgnoreActor, const bool* bWasVisible, int32* UserData) const
{
	ECollisionChannel DefaultSightCollisionChannel = ECollisionChannel::ECC_Visibility;
	UWorld* pWorld = GetWorld();
	const FName TraceTag("VisiblityTraceTag");
	pWorld->DebugDrawTraceTag = TraceTag;
	FCollisionQueryParams params = FCollisionQueryParams(SCENE_QUERY_STAT(AILineOfSight), true, IgnoreActor);
	params.TraceTag = TraceTag;
	FHitResult HitResult;
	const bool bHit = pWorld->LineTraceSingleByChannel(HitResult, ObserverLocation, GetActorLocation()
		, DefaultSightCollisionChannel
		, params);
	if (bHit && HitResult.GetActor() == this)
	{
		OutSeenLocation = HitResult.Location;
		return true;
	}
	return false;
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
	const FVector forwardBackwardPredictionVector = actorQuat.GetForwardVector() * m_ForwardPredictionLengthMax; 
	const FVector leftRightPredictionVector = actorQuat.GetRightVector() * m_LeftRightPredictionLengthMax;
	const FVector upDownPredictionVector = actorQuat.GetUpVector() * m_UpDownPredictionLengthMax;
		
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

	AngleTestResult resultPitch = DoAngleTest(GetActorLocation(), forwardPoint, GetActorLocation(), backwardPoint, m_ForwardPredictionLengthMax, m_ForwardPredictionLengthMax, m_ForwardPredictionLengthMin);
	if (!resultPitch.wasHitTerrain)
		resultPitch = DoAngleTest(forwardPoint, forwardDownPoint, backwardPoint, backwardDownPoint,  m_ForwardPredictionLengthMax, m_UpDownPredictionLengthMax, m_UpDownPredictionLengthMin);
	if (!resultPitch.wasHitTerrain)
		resultPitch.desiredAngle = 1.0f;
	// AngleTestResult resultRoll = DoAngleTest(GetActorLocation(), rightPoint, GetActorLocation(), leftPoint, m_LeftRightPredictionLengthMax, m_LeftRightPredictionLength, m_LegAlignmentBodyRotationRollSensitivity);
	// add an additional pitch based on the above result (?)
	// pitch is y component
	GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Yellow, *FString::Printf(TEXT("Pitch Angle: %f"), resultPitch.desiredAngle));

	float pitchAngularVelocity =  -FMath::Sign(resultPitch.desiredAngle) * m_TurnSpeedByAngularOffsetToDesired.EditorCurveData.Eval(abs(resultPitch.desiredAngle));

	
	
	FQuat additionalQuat = FQuat::MakeFromEuler(FVector(0.0f, pitchAngularVelocity * DeltaTime, 0.0f));

	FQuat desiredQuat = actorQuat * additionalQuat;
	if(SchismDebug.GetValueOnGameThread() > 0)
	{
		DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + desiredQuat.GetUpVector() * 100.0f, FColor::Green, false, -1, 0, 10);
		DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + desiredQuat.GetForwardVector() * 100.0f, FColor::Red, false, -1, 0, 10);
		DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + desiredQuat.GetRightVector() * 100.0f, FColor::Blue, false, -1, 0, 10);
	}
	// FQuat newActorQuat = UKismetMathLibrary::QuaternionSpringInterp(actorQuat, desiredQuat,  m_RotSpringState, m_BodyRotationSpeed, m_BodyRotationDamping, DeltaTime);
	// rotate until rightLength and leftLength are equal
	// so, essentially, want to measure the difference
	SetActorRotation(desiredQuat);
	// if there's uneven terrain ahead of us, leftLength and rightLength will capture that data
}

