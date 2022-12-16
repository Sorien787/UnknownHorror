// Fill out your copyright notice in the Description page of Project Settings.


#include "TensionSubsystem.h"

#include "LurkLocationActor.h"

TStatId UTensionSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UTensionSubsystem, STATGROUP_Tickables);
}

void UTensionSubsystem::Tick(float DeltaTime)
{
	m_fCurrentTension -= m_TensionDecayCurve.EditorCurveData.Eval(m_fCurrentTension / m_fMaximumTension) * DeltaTime;
}

void UTensionSubsystem::AddSingleTensionInstance(float addTension)
{
	m_fCurrentTension += addTension;
	m_fCurrentTension = FMath::Min(m_fCurrentTension, m_fMaximumTension);
}

float UTensionSubsystem::GetCurrentTension() const
{
	return m_fCurrentTension / m_fMaximumTension;
}

void UTensionSubsystem::RegisterLurkLocation(ALurkLocationActor* pLurkActor)
{
	m_LurkLocations.push_back(pLurkActor);
}

ALurkLocationActor* UTensionSubsystem::GetLurkLocation(uint8 type, FVector location) const
{
	ALurkLocationActor* returnActor = nullptr;
	float bestDistance = FLT_MAX;
	for (size_t nLurkLocIndex = 0; nLurkLocIndex < m_LurkLocations.size(); nLurkLocIndex++)
	{
		ALurkLocationActor* pLurkActor = m_LurkLocations[nLurkLocIndex];
		if (!pLurkActor->IsValidLurkType((EAIType(type))))
			continue;
		
		const float dist = (pLurkActor->GetActorLocation()-location).Length();
		if (dist > bestDistance)
			continue;
		returnActor = pLurkActor;
		bestDistance = dist;
	}
	return returnActor;
}
