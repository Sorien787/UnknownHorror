// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractableInterface.h"

// Add default functionality here for any IInteractableInterface functions that are not pure virtual.

FVector IInteractableInterface::GetInteractableLocation_Implementation() const
{
	return FVector::Zero();
}

void IInteractableInterface::IsInteractionAvailable_Implementation(const int interactorId, const TScriptInterface<IInteractionComponentInterface>& InteractionUser, bool& returnResult)
{
}

void IInteractableInterface::OnInteractWithUsingInteractable_Implementation(const int interactorId, const TScriptInterface<IInteractionComponentInterface>& pInteractionUser)
{
	return;
}

void IInteractableInterface::GetPossibleAvailableInteractions_Implementation(const TScriptInterface<IInteractionComponentInterface>& pInteractionUser, TArray<int>& result)
{
	return;
}

void IInteractableInterface::OnInteractWithInteractorId_Implementation(const int interactorId, const TScriptInterface<IInteractionComponentInterface>& pInteractionUser, bool& result)
{
	return;
}

FTransform IInteractableInterface::GetDesiredTransformForInteraction_Implementation(const int interactorId,const InteractionUserType pInteractionUser)
{
	return FTransform::Identity;
}

bool IInteractableInterface::TryCancelInteraction_Implementation(const TScriptInterface<IInteractionComponentInterface>& pInteractionUser)
{
	return false;
}
