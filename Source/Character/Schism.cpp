// Fill out your copyright notice in the Description page of Project Settings.


#include "Schism.h"

#include "Kismet/KismetMathLibrary.h"

static TAutoConsoleVariable<int32> SchismDebug(
	TEXT("SchismDebug"),
	0,
	TEXT("Shows Schism debug visualization.\n"),
	ECVF_Scalability | ECVF_RenderThreadSafe);

#pragma optimize("", off)
FQuat ASchism::GetDesiredBodyRotation(float DeltaTime)
{
	FVector forwardVector = (m_pFrontRightLeg->GetCurrentFootPosition() - m_pBackRightLeg->GetCurrentFootPosition() + m_pFrontLeftLeg->GetCurrentFootPosition() - m_pBackLeftLeg->GetCurrentFootPosition())/2.0f;
	forwardVector = forwardVector.GetUnsafeNormal();
	

	FVector rightVector = (m_pFrontRightLeg->GetCurrentFootPosition() - m_pFrontLeftLeg->GetCurrentFootPosition() + m_pBackRightLeg->GetCurrentFootPosition() - m_pBackLeftLeg->GetCurrentFootPosition())/2.0f;
	rightVector = rightVector.GetUnsafeNormal();

	const FVector upVector = forwardVector.Cross(rightVector);

	
	rightVector = upVector.Cross(forwardVector);

	if(SchismDebug.GetValueOnGameThread() > 0)
	{
		DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + upVector * 100.0f, FColor::Green);
		DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + forwardVector * 100.0f, FColor::Red);
		DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + rightVector * 50.0f, FColor::Blue);
	}
	
	FQuat desiredQuat = UKismetMathLibrary::MakeRotationFromAxes(forwardVector,rightVector, upVector).Quaternion();
	
	const FQuat currentQuat = GetOwner()->GetActorRotation().Quaternion();

	// only change the relative yaw and the relative roll
	// relative pitch should stay the same

	const FQuat currentQuatToDesiredQuat = currentQuat.Inverse() * desiredQuat;

	FVector eulerRots = currentQuatToDesiredQuat.Euler();

	eulerRots.X *= m_x;
	eulerRots.Y *= m_y;
	eulerRots.Z *= m_z;
	//  roll X, pitch Y, yaw Z
	desiredQuat = currentQuat * FQuat::MakeFromEuler(eulerRots);
	
	return UKismetMathLibrary::QuaternionSpringInterp(currentQuat, desiredQuat,  m_RotSpringState, m_BodyRotationSpeed, m_BodyRotationDamping, DeltaTime);
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
	 const FQuat desiredRotation = GetDesiredBodyRotation(DeltaTime);
	
	SetActorRotation(desiredRotation.Rotator(), ETeleportType::None);

	
}

