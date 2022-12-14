// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractableInterface.h"

// Add default functionality here for any IInteractableInterface functions that are not pure virtual.

void IInteractableInterface::IsInteractionAvailableOverride_Implementation(const int interactorId, const UInteractionUserComponent* pInteractionUser,  bool& result)//const UInteractionUserComponent* pInteractionUser, const int interactorId)
{
	return;
}

void IInteractableInterface::OnInteractWithInteractorId2_Implementation(const int interactorId, const UInteractionUserComponent* pInteractionUser, bool& result)
{
	return;
}

FTransform IInteractableInterface::GetDesiredTransformForInteraction_Implementation(const int interactorId,
	const UInteractionUserComponent* pInteractionUser)
{
	return FTransform::Identity;
}
