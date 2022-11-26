// Fill out your copyright notice in the Description page of Project Settings.


#include "TensionSubsystem.h"

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
