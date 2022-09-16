// Fill out your copyright notice in the Description page of Project Settings.


#include "LegComponent.h"

#include "UnrealUtilities.h"


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


bool ULegComponent::VerifyCanUnplant(float StepDistance)
{
	if (!m_bIsPlanted)
		return false;
	m_bHasMovedEnoughToPlant |= FVector::Distance(m_StartFootPosition, m_TracePosition) > StepDistance;
	return m_bHasMovedEnoughToPlant;
}

void ULegComponent::SetNewFootTargetLocation(float RayCastLength)
{
	const FVector legDefault = GetComponentLocation();
	const FVector vectorDiff = RayCastLength * GetLegUpVector();
	const FVector lineTraceStart = legDefault + vectorDiff;
	const FVector lineTraceEnd = legDefault - vectorDiff;
	
	static FName TraceTag = TEXT("LegTrace");
	FCollisionQueryParams traceParams(TraceTag, false, GetOwner());

	FHitResult outHit;
	const bool hasHit = GetWorld()->LineTraceSingleByChannel(
		OUT outHit,
		lineTraceStart,
		lineTraceEnd,
		ECollisionChannel::ECC_WorldStatic,
		traceParams);
	if (!hasHit)
	{
		m_TracePosition = legDefault;
		RenderDebugInfo();
		return;
	}
	RenderDebugInfo();
	m_TracePosition = outHit.Location;
}

void ULegComponent::RenderDebugInfo() const
{
	DrawDebugSphere(GetWorld(), m_TracePosition, 10, 26, FColor::White, false);
	const FColor startendrenderColor = m_bIsPlanted ? FColor::Blue : FColor::Red;
	DrawDebugSphere(GetWorld(), m_StartFootPosition, 5, 26, startendrenderColor, false);
	DrawDebugSphere(GetWorld(), m_EndFootPosition, 5, 26, startendrenderColor, false);
	const FColor renderColor = m_bHasMovedEnoughToPlant ? FColor::Blue : FColor::Red;
	DrawDebugLine(GetWorld(), m_StartFootPosition, m_EndFootPosition, renderColor, false);
	DrawDebugSphere(GetWorld(), m_CurrentFootPosition, 15, 26, FColor::Purple, false);
}


void ULegComponent::UpdateFootState(float StepTime, float totalStepHeight)
{
	if (m_bIsPlanted)
		return;
	
	const float percentage = (GetWorld()->GetTimeSeconds() - m_fUnplantStartTime) / StepTime;
	const float dist = m_DistanceByTime.EditorCurveData.Eval(percentage);
	const float height = m_HeightByTime.EditorCurveData.Eval(percentage);

	m_CurrentFootPosition = m_StartFootPosition + (m_EndFootPosition - m_StartFootPosition) * dist + totalStepHeight * height * GetLegUpVector();
	if (percentage < 1.0f)
		return;
	m_bIsPlanted = true;
	m_CurrentFootPosition = m_EndFootPosition;
	m_StartFootPosition = m_CurrentFootPosition;
	m_fPlantStartTime = GetWorld()->GetTimeSeconds();
}

float ULegComponent::GetTimeSinceLastPlant() const
{
	if (!m_bIsPlanted)
		return 0.0f;
	return GetWorld()->GetTimeSeconds() - m_fPlantStartTime;
	
}

float ULegComponent::GetPlantedQuantity(float StepTime) const
{
	if (m_bIsPlanted)
		return 0.0f;
	return 1 - (GetWorld()->GetTimeSeconds()-m_fUnplantStartTime) / StepTime;
}


FVector ULegComponent::GetLegUpVector() const
{
	return FVector::UpVector;
}

void ULegComponent::UnplantFoot()
{
	m_bIsPlanted = false;
	m_EndFootPosition = m_TracePosition;
	m_fUnplantStartTime = GetWorld()->GetTimeSeconds();
	m_bHasMovedEnoughToPlant = false;
}


