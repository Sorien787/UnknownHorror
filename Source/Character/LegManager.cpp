// Fill out your copyright notice in the Description page of Project Settings.


#include "LegManager.h"

#include "WallClimbingMovementComponent.h"
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

void ULegManager::SetMovementComponent(UPawnMovementComponent* movementComponent)
{
	m_pMovementComponent = movementComponent;
}

void ULegManager::UpdateLegs(float DeltaTime)
{

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
	const float stepDelayTime = m_StepTimeDelayByMovementSpeed.EditorCurveData.Eval(actorSpeedNormalized);
	const float stepVelocity = m_MinimumVelocityForStepTaking;
	const float stepHeight = m_StepHeightByMovementSpeed.EditorCurveData.Eval(actorSpeedNormalized);
	const bool velocityStepRequired = m_pMovementComponent->Velocity.SquaredLength() > stepVelocity * FMath::Abs(stepVelocity);
	
	const ULegComponent* bestLegToLift = m_controllableLegComponents[0];
	
	for (const auto& leg : m_controllableLegComponents)
	{
		leg->TrySetNewFootTargetLocation(m_LegRaycastProfiles, actorTransform, actorVelocityDirection, stepTime, stepHeight, extraStepDistanceByVelocity);
		
		if (!leg->GetIsPlanted())
			continue;

		// or if leg, due to other legs being better candidates, cant unplant, early out
		if (bestLegToLift && (bestLegToLift->GetSqDistanceFromStartPoint() > leg->GetSqDistanceFromStartPoint()))
			continue;
		
		bestLegToLift = leg;
	}

	// if leg state means no new leg can unplant, early out
	
	if (!bestLegToLift || (bestLegToLift->GetSqDistanceFromStartPoint() < m_MinimumStepDistance && !velocityStepRequired))
		return;

	// for a stagger, both feet can be down at the same time. In fact, we WANT both feet down at the same time.
	// for a sprint, both feet can leave the ground as long as they're in rhythm.
	// figure out the last leg(s) that lifted
	//

	const ULegComponent* lastLegToLift = nullptr;
	
	for (const auto& leg : m_controllableLegComponents)
	{
		if (leg == bestLegToLift)
			continue;

		if (lastLegToLift && lastLegToLift->GetTimeSinceLastPlant() < leg->GetTimeSinceLastPlant())
			continue;
		
		lastLegToLift = leg;
	}

	// use LastLegToLift's time to figure out when we lift this leg
	// it's possible that LastLegToLift is still negative (I.E, still in the air)
	// that's fine - just use the quantity and determine if it's larger.

	if (lastLegToLift && lastLegToLift->GetTimeSinceLastPlant() < stepDelayTime)
		return;


	// if this leg is lifting, collate all the legs that are in the same group and lift those, too.
	
	for (const auto& leg : m_controllableLegComponents)
	{
		if(leg->GetLegGrouping() != bestLegToLift->GetLegGrouping())
			continue;
		
		leg->UnplantFoot();
	}
	
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

