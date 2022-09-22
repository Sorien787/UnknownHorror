// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractionTriggerInterface.h"
#include "InteractionUserComponent.h"

// Add default functionality here for any IInteractionTriggerInterface functions that are not pure virtual.
bool IInteractionTriggerInterface::GetCanInteract(const UInteractionUserComponent* pUser) const
{
	if (!m_bIsCurrentlyActive || !GetActorTriggerConditionMet(pUser->GetOwner()->GetTransform(), pUser->GetOwner()->GetVelocity()))
		return false;

	return m_pInteractableInterface->IsInteractionAvailable(pUser, GetInteractorId());
}


