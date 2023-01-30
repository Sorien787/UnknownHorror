// Fill out your copyright notice in the Description page of Project Settings.


#include "Interaction/InteractionComponentInterface.h"

#include "InteractableInterface.h"

void UInteractionComponentBase::OnInteractionStarted(const TScriptInterface<IInteractableInterface> interf)
{
	m_bIsInteracting = true;
	m_pCurrentlyInteractingInterface = interf;
}

void UInteractionComponentBase::OnInteractionFinished(const TScriptInterface<IInteractableInterface> interf)
{
	m_bIsInteracting = false;
	m_pCurrentlyInteractingInterface =  TScriptInterface<IInteractableInterface>();
}

bool UInteractionComponentBase::TryCancelCurrentInteraction_Implementation()
{
	if (!m_bIsInteracting)
		return true;

	return m_pCurrentlyInteractingInterface->Execute_TryCancelInteraction(m_pCurrentlyInteractingInterface->GetThisObject(), this);
}
