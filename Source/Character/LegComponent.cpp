// Fill out your copyright notice in the Description page of Project Settings.


#include "LegComponent.h"

#include "../Common/UnrealUtilities.h"
#include "Kismet/KismetSystemLibrary.h"


static TAutoConsoleVariable<int32> LegDebug(
	TEXT("LegDebug"),
	0,
	TEXT("Shows Leg debug visualization.\n"),
	ECVF_Scalability | ECVF_RenderThreadSafe);


static TAutoConsoleVariable<int32> LegDebugRaycast(
	TEXT("LegDebugRaycast"),
	0,
	TEXT("Shows Leg debug visualization.\n"),
	ECVF_Scalability | ECVF_RenderThreadSafe);


static TAutoConsoleVariable<int32> LegDebugOverstep(
	TEXT("LegDebugOverstep"),
	0,
	TEXT("Shows Leg debug visualization.\n"),
	ECVF_Scalability | ECVF_RenderThreadSafe);

ULegComponent::ULegComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void ULegComponent::BeginPlay()
{
	Super::BeginPlay();
	
	m_CurrentFootPosition = GetComponentLocation();
	m_StartFootPosition = GetComponentLocation();
	m_EndFootPosition = GetComponentLocation();
	m_TracePosition = GetComponentLocation();

}

bool ULegComponent::TrySetNewFootTargetLocation(const TArray<FLegRaycastProfile>& raycastProfile, const FTransform& creatureTransform, const FVector& creatureVelocity, float stepTime, float stepDistance, float stepHeight, float velocityStepDistance)
{
	if (LegDebug.GetValueOnGameThread() > 0)
		RenderDebugInfo();

	if (m_bIsFootPlanted)
	{
		m_stepUpVector = creatureTransform.Rotator().Quaternion().GetUpVector();
		m_minDistanceForStep = stepDistance;
		m_timeTakenForStep = stepTime;
		m_lastStepHeight = stepHeight;
	}
	// initialize data
	const FQuat creatureQuat = creatureTransform.Rotator().Quaternion();
	const FVector rightVector = creatureQuat.GetRightVector();
	const FVector legDefault = GetComponentLocation();
	const float rightVecSign = FMath::Sign(rightVector.Dot(legDefault - creatureTransform.GetLocation()));
	FVector movementDirectionOffset = FVector::ZeroVector;
	const float movementSpeed = creatureVelocity.Length();
	// set movement speed offset if we have a valid movement speed
	if (movementSpeed > FLT_EPSILON)
	{
		const FVector movementDirection = creatureVelocity / movementSpeed;
		movementDirectionOffset = movementDirection * velocityStepDistance;
	}
	FHitResult outHit;
	bool bHasHit = false;

	// try to raycast to find (a) valid foot position(s)
	// uses raycast profile to determine angle, and type of raycast
	for (int i = 0; i < raycastProfile.Num(); i++)
	{
		const FLegRaycastProfile& thisRaycast = raycastProfile[i];
		
		const float& lateralOffsetLength = thisRaycast.lateralDistance;
		const float& useSphereCast = thisRaycast.raycastSize;
		const float& raycastLength = thisRaycast.raycastLength;
		const float& angle = m_bReverseRaycastDirection ? -thisRaycast.angleTowardMovementDir : thisRaycast.angleTowardMovementDir;

		const FVector lateralOffset = rightVecSign * rightVector * lateralOffsetLength;
		const FVector raycastCenter = legDefault + lateralOffset + movementDirectionOffset;

		const FVector raycastDirection = FQuat::MakeFromEuler(FVector(0.0f, angle, 0.0f)) * creatureQuat.GetUpVector();
		const FVector raycastHalfSpan = (raycastLength / 2.0f) * raycastDirection;
		const FVector lineTraceStart = raycastCenter + raycastHalfSpan;
		const FVector lineTraceEnd = raycastCenter - raycastHalfSpan;

		if (LegDebug.GetValueOnGameThread() > 0)
		{
			DrawDebugDirectionalArrow(GetWorld(), creatureTransform.GetLocation(), creatureTransform.GetLocation() + raycastHalfSpan, raycastLength/2.0f, FColor::Red );
			DrawDebugSphere(GetWorld(), lineTraceStart, 5.0f, 16, FColor::Green);
			DrawDebugSphere(GetWorld(), lineTraceEnd, 5.0f, 16, FColor::Red);
		}

		if (useSphereCast > FLT_EPSILON)
		{
			TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypesArray;
			TArray<AActor*> IgnoredActors;
			
			IgnoredActors.Reserve(1);
			IgnoredActors.Add(GetOwner());
			
			ObjectTypesArray.Reserve(1);
			ObjectTypesArray.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic));

			EDrawDebugTrace::Type drawTrace = EDrawDebugTrace::None;
			if (LegDebugRaycast.GetValueOnGameThread() > 0)
				drawTrace = EDrawDebugTrace::ForOneFrame;
			
			bHasHit = UKismetSystemLibrary::SphereTraceSingleForObjects(
				GetWorld(),
				lineTraceStart,
				lineTraceEnd, 
				useSphereCast,
				ObjectTypesArray,
				false,
				IgnoredActors,
				drawTrace,
				outHit,
				true
				, FColor::Red,
				FColor::Green,
				-1);
		}
		else
		{
			static FName TraceTag = TEXT("LegTrace");
			FCollisionQueryParams traceParams(TraceTag, false, GetOwner());
			
			bHasHit = GetWorld()->LineTraceSingleByChannel(
				OUT outHit,
				lineTraceStart,
				lineTraceEnd,
				ECollisionChannel::ECC_WorldStatic,
				traceParams);

			
			if (LegDebugRaycast.GetValueOnGameThread() > 0)
				UnrealUtilities::DrawLineTrace(GetWorld(), bHasHit, lineTraceStart, lineTraceEnd, outHit);
		}

		if (bHasHit)
			break;
	}

	if (!bHasHit)
		return false;

	// store historical path point so that it can be used to figure out how high we need to raise our leg when it moves
	m_historicalPathTracePoints.emplace_back(m_TracePosition);
	m_TracePosition = outHit.Location;
	return true;
}

void ULegComponent::RenderDebugInfo() const
{
	DrawDebugSphere(GetWorld(), m_TracePosition, 10, 26, FColor::White, false);
	const FColor startEndRenderColor = m_bIsFootPlanted ? FColor::Blue : FColor::Red;
	
	DrawDebugSphere(GetWorld(), m_StartFootPosition, 5, 26, startEndRenderColor, false);
	DrawDebugSphere(GetWorld(), m_EndFootPosition, 5, 26, startEndRenderColor, false);
	
	const FColor renderColor = m_bHasMovedEnoughToUnplant ? FColor::Blue : FColor::Red;
	DrawDebugLine(GetWorld(), m_StartFootPosition, m_EndFootPosition, renderColor, false);
	DrawDebugSphere(GetWorld(), m_CurrentFootPosition, 15, 26, FColor::Purple, false);
}

float ULegComponent::GetFootHeightDefaultRelative(FVector upVector) const
{
	const FVector defaultToCurrent = m_CurrentFootPosition - GetComponentLocation();
	return upVector.Dot(defaultToCurrent);
}

void ULegComponent::UpdateFootState()
{
	if (m_bIsFootPlanted)
	{
		m_bHasMovedEnoughToUnplant |= FVector::Distance(m_StartFootPosition, m_TracePosition) > m_minDistanceForStep;
	}
	else
	{
		const float stepCompletionPercentage = (GetWorld()->GetTimeSeconds() - m_fUnplantStartTime) / m_timeTakenForStep;
		const float stepHorizontalTime = m_DistanceByTime.EditorCurveData.Eval(stepCompletionPercentage);
		const float stepVerticalTime = m_HeightByTime.EditorCurveData.Eval(stepCompletionPercentage);

		const FVector horizontalStepAddition = (m_EndFootPosition - m_StartFootPosition) * stepHorizontalTime;
		const FVector verticalStepAddition =  (m_lastStepHeight + m_additionalStepHeight) * stepVerticalTime * m_stepUpVector;
		m_CurrentFootPosition = m_StartFootPosition + horizontalStepAddition + verticalStepAddition;

		if (stepCompletionPercentage < 1.0f)
			return;
		
		m_bIsFootPlanted = true;
		m_CurrentFootPosition = m_EndFootPosition;
		m_StartFootPosition = m_CurrentFootPosition;
		m_fPlantStartTime = GetWorld()->GetTimeSeconds();
	}
}

bool ULegComponent::CanUnplant() const
{
	return m_bHasMovedEnoughToUnplant;
}

float ULegComponent::GetLegIKValidity() const
{
	return m_fLegIKValidity;
}

float ULegComponent::GetTimeSinceLastPlant() const
{
	if (!m_bIsFootPlanted)
		return 0.0f;
	return GetWorld()->GetTimeSeconds() - m_fPlantStartTime;
}

float ULegComponent::GetPlantedQuantity(float StepTime) const
{
	if (m_bIsFootPlanted)
		return 0.0f;
	return 1 - (GetWorld()->GetTimeSeconds()-m_fUnplantStartTime) / StepTime;
}

FVector ULegComponent::GetCurrentFootPosition() const
{
	return m_CurrentFootPosition;
}

void ULegComponent::UnplantFoot()
{
	// we're unplanting from start to finish
	// iterate along the line, see if we're far below the line from start to finish
	m_EndFootPosition = m_TracePosition;
	
	m_additionalStepHeight = 0.0f;
	for (auto pathTracePoint = m_historicalPathTracePoints.begin(); pathTracePoint < m_historicalPathTracePoints.end(); ++pathTracePoint)
	{
		// find normal vector to line
		const FVector startToEnd = m_EndFootPosition - m_StartFootPosition;

		const FVector startToPathPoint = *pathTracePoint - m_StartFootPosition;

		// ignore when start point is same as path point in queue
		// ignore when we might have traced beyond current foot position
		if ((startToPathPoint.SquaredLength() < FLT_EPSILON) || (startToPathPoint.SquaredLength() > startToEnd.SquaredLength()))
			continue;

		const FVector startToPathPointNormalized = startToPathPoint.GetUnsafeNormal();
		const FVector startToEndNormalized = startToEnd.GetUnsafeNormal();

		const float dotProd = FVector::DotProduct(startToEndNormalized, startToPathPoint);
		const FVector closestToPathPointOnLine = m_StartFootPosition + startToEndNormalized * dotProd;
		const FVector pointOnLineToPathPoint = *pathTracePoint - closestToPathPointOnLine;

		// height above the start-end using the foot up vector as vertical that the terrain has trace
		const float heightAbove = FVector::DotProduct(m_stepUpVector, pointOnLineToPathPoint);

		if (LegDebugOverstep.GetValueOnGameThread() > 0)
		{
			FColor drawColor = heightAbove > 0 ? FColor::Red : FColor::Green;
			DrawDebugLine(GetWorld(), closestToPathPointOnLine, *pathTracePoint, drawColor, false, m_timeTakenForStep);			
		}
		// additional height to trace when we're trying to take a step
		m_additionalStepHeight = FMath::Max(heightAbove, m_additionalStepHeight);
	}
	
	m_historicalPathTracePoints.clear();
	m_bIsFootPlanted = false;
	m_fUnplantStartTime = GetWorld()->GetTimeSeconds();
	m_bHasMovedEnoughToUnplant = false;
}

