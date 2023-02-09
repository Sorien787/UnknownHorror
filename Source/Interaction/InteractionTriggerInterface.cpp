// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractionTriggerInterface.h"
#include "InteractionUserComponent.h"
// Add default functionality here for any IInteractionTriggerInterface functions that are not pure virtual.
bool IInteractionTriggerInterface::GetCanInteract(IInteractionComponentInterface* pUser)
{
	AActor* pOwner = pUser->GetInteractionOwner();
	
	if (!m_bIsCurrentlyActive || !GetActorTriggerConditionMet(pOwner->GetTransform(), pOwner->GetVelocity()))
		return false;

	bool result = false;
	TScriptInterface<IInteractionComponentInterface> interface;
	interface.SetInterface(pUser);
	interface.SetObject(pUser->GetComponentObject());
	m_pInteractableInterface->Execute_IsInteractionAvailable(m_pInteractableInterface->GetThisObject() ,GetInteractorId(), interface, result);
	return result;
}


