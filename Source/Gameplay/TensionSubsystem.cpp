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

void UTensionSubsystem::RegisterLurkLocation(const TScriptInterface<IInteractableInterface>& pLurkActor)
{
	m_LurkLocations.push_back(pLurkActor.GetInterface());
}

void UTensionSubsystem::RegisterEscapeLocation(const TScriptInterface<IInteractableInterface>& pEscapeActor)
{
	m_EscapeLocations.push_back(pEscapeActor.GetInterface());
}

TScriptInterface<IInteractableInterface> UTensionSubsystem::GetLurkLocation(InteractionUserType type, FVector location) const
{
	IInteractableInterface* returnActor = nullptr;
	float bestDistance = FLT_MAX;
	TArray<int> possibleAvailableInteractions;
	for (size_t nLurkLocIndex = 0; nLurkLocIndex < m_LurkLocations.size(); nLurkLocIndex++)
	{
		IInteractableInterface* pLurkActor = m_LurkLocations[nLurkLocIndex];

		pLurkActor->Execute_GetPossibleAvailableInteractions(pLurkActor->GetThisObject(), type ,possibleAvailableInteractions);

		if (possibleAvailableInteractions.Num() == 0)
			continue;

		const float dist = (pLurkActor->Execute_GetInteractableLocation(pLurkActor->GetThisObject()) -location).Length();
		if (dist > bestDistance)
			continue;
		returnActor = pLurkActor;
		bestDistance = dist;
	}
	TScriptInterface<IInteractableInterface> returnInterface;
	if (returnActor)
	{
		returnInterface.SetInterface(returnActor);
		returnInterface.SetObject(returnActor->GetThisObject());
	}
	return returnInterface;
}

TScriptInterface<IInteractableInterface> UTensionSubsystem::GetEscapeLocation(InteractionUserType type,
	FVector location) const
{
	IInteractableInterface* returnActor = nullptr;
	float bestDistance = FLT_MAX;
	TArray<int> possibleAvailableInteractions;
	for (size_t nLurkLocIndex = 0; nLurkLocIndex < m_EscapeLocations.size(); nLurkLocIndex++)
	{
		IInteractableInterface* pEscapeActor = m_EscapeLocations[nLurkLocIndex];

		pEscapeActor->Execute_GetPossibleAvailableInteractions(pEscapeActor->GetThisObject(), type ,possibleAvailableInteractions);

		if (possibleAvailableInteractions.Num() == 0)
			continue;

		const float dist = (pEscapeActor->Execute_GetInteractableLocation(pEscapeActor->GetThisObject()) -location).Length();
		if (dist > bestDistance)
			continue;
		returnActor = pEscapeActor;
		bestDistance = dist;
	}
	TScriptInterface<IInteractableInterface> returnInterface;
	if (returnActor)
	{
		returnInterface.SetInterface(returnActor);
		returnInterface.SetObject(returnActor->GetThisObject());
	}
	return returnInterface;
}

void UTensionSubsystem::OnEntityEscaped(AActor* entity)
{
}
