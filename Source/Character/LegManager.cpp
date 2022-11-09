// Fill out your copyright notice in the Description page of Project Settings.


#include "LegManager.h"

#include "GameFramework/PawnMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

static TAutoConsoleVariable<int32> LegManagerDebug(
	TEXT("LegManagerDebug"),
	0,
	TEXT("Shows Leg debug visualization.\n"),
	ECVF_Scalability | ECVF_RenderThreadSafe);


// Sets default values for this component's properties
ULegManager::ULegManager()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void ULegManager::BeginPlay()
{
	Super::BeginPlay();
	
	GetOwner()->GetComponents<ULegComponent>(m_controllableLegComponents);


	TArray<UPawnMovementComponent*> movementComponents;
	GetOwner()->GetComponents<UPawnMovementComponent>(movementComponents);
	
	if (movementComponents.Num() < 1)
		return;

	m_fMaxPawnSpeed = movementComponents[0]->GetMaxSpeed();
	
	// ...
	
}

void ULegManager::UpdateDesiredBodyHeightDefaultRelative(float DeltaTime)
{
	// Raycast at some point between body and legs
	// find how far it is to the floor
	// choose the closest point
	// use a capsule cast, since then it wont miss objects

	if (!m_pBoxComponentForGroundCollision)
		return;
	
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypesArray;
	TArray<AActor*> IgnoredActors;
	IgnoredActors.Reserve(1);
	IgnoredActors.Add(GetOwner());
	ObjectTypesArray.Reserve(1);
	ObjectTypesArray.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic));

	EDrawDebugTrace::Type drawTrace = EDrawDebugTrace::None;
	if (LegManagerDebug.GetValueOnGameThread() > 0)
		drawTrace = EDrawDebugTrace::ForOneFrame;
			
	FHitResult outHit;
	float desiredOffset = 0.0f;
	const FVector axis = m_pBoxComponentForGroundCollision->GetComponentRotation().Quaternion().GetUpVector();
	const FVector start = m_pBoxComponentForGroundCollision->GetComponentLocation() + m_pBoxComponentForGroundCollision->GetComponentRotation().Quaternion() * (FVector(0, m_BoxTraceHeightUp, 0));
	const FVector end = start - m_pBoxComponentForGroundCollision->GetComponentRotation().Quaternion() * (FVector(0, m_BoxTraceHeightDown, 0));
	const bool bHasHit = UKismetSystemLibrary::BoxTraceSingleForObjects(
		GetWorld(),
		start,
		end,
		m_pBoxComponentForGroundCollision->GetScaledBoxExtent(),
		m_pBoxComponentForGroundCollision->GetComponentRotation(),
		ObjectTypesArray,
		false,
		IgnoredActors,
		drawTrace,
		outHit,
		true,
		FLinearColor::Red,
		FLinearColor::Green,
		-1);

	if (bHasHit)
		desiredOffset = m_BoxTraceHeightUp - outHit.Distance;

	m_fCurrentOffsetHeight = UKismetMathLibrary::FloatSpringInterp(m_fCurrentOffsetHeight, desiredOffset, m_fCurrentBodyTranslationState, m_BodyTranslationSpeed, m_BodyTranslationDamping, DeltaTime);
}

void ULegManager::UpdateLegs(float DeltaTime)
{
	ULegComponent* bestLeg = nullptr;

	float totalLegUnplantQuantity = 0.0f;

	const FTransform& actorTransform = GetOwner()->GetTransform();
	const FVector actorVelocity = GetOwner()->GetVelocity();
	const float actorSpeed = actorVelocity.Length();

	FVector actorVelocityDirection = GetOwner()->GetActorQuat().GetForwardVector();
	if (actorSpeed > FLT_EPSILON)
	{
		actorVelocityDirection = actorVelocity / actorSpeed;
	}
	
	float actorSpeedNormalized = actorSpeed;
	if (m_fMaxPawnSpeed > FLT_EPSILON)
	{
		actorSpeedNormalized/=m_fMaxPawnSpeed;		
	}
	
	const float extraStepDistanceByVelocity = m_ExtraStepDistanceByMovementSpeed.EditorCurveData.Eval(actorSpeedNormalized);
	const float stepTime = m_StepTimeByMovementSpeed.EditorCurveData.Eval(actorSpeedNormalized);
	const float stepDistance = m_StepDistanceByMovementSpeed.EditorCurveData.Eval(actorSpeedNormalized);
	const float stepHeight = m_StepHeightByMovementSpeed.EditorCurveData.Eval(actorSpeedNormalized);
	
	for (const auto& leg : m_controllableLegComponents)
	{
		leg->TrySetNewFootTargetLocation(m_LegRaycastProfiles, actorTransform, actorVelocityDirection, stepTime, stepDistance, stepHeight, extraStepDistanceByVelocity);
		totalLegUnplantQuantity += leg->GetTimeSinceLastPlant();

		// if leg internally can't unplant, early out
		leg->UpdateFootState();

		if (!leg->CanUnplant())
			continue;

		// or if leg, due to other legs being better candidates, cant unplant, early out
		if (bestLeg && (bestLeg->GetTimeSinceLastPlant() > leg->GetTimeSinceLastPlant()))
			continue;
		
		bestLeg = leg;
	}

	// if leg state means no new leg can unplant, early out
	if (totalLegUnplantQuantity < LegUnplantThreshold)
		return;

	if (!bestLeg)
		return;

	bestLeg->UnplantFoot();
}

FVector ULegManager::GetCurrentRaisedBodyPosition() const
{
	const FVector axis = GetComponentRotation().Quaternion().GetUpVector();
	return  GetComponentLocation()  + axis * m_fCurrentOffsetHeight;
}


// Called every frame
void ULegManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateLegs(DeltaTime);

	UpdateDesiredBodyHeightDefaultRelative(DeltaTime);
}

void ULegManager::SetBoxForBodyCollision(UBoxComponent* boxComponent)
{
	m_pBoxComponentForGroundCollision = boxComponent;
}

