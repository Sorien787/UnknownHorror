// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractableObject.h"

#include "UnrealUtilities.h"
#include "InteractionUserComponent.h"


void AInteractableObject::BeginPlay()
{
	Super::BeginPlay();
}

AInteractableObject::AInteractableObject()
{

}


void AInteractableObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// when picked up, test for dropable interactions
	// and set an internal flag
}

void AInteractableObject::OnInteractionStarted(UInteractionUserComponent* pInteractionUser, int interactorId)
{
	Super::OnInteractionStarted(pInteractionUser, interactorId);
}

// we could do a sort of interaction animation callback?
// so, for this one, we get a callback when the hand IK touches, and attach it to the hand
// for doors, we get a callback when the hand touches the door, then the door opens and the hand IK follows while the skeleton

// for pickup/put down:
// stage 1: start anim 
void AInteractableObject::OnInteractionFinished(UInteractionUserComponent* pInteractionUser)
{
	
	switch(m_CurrentState)
	{
		// move into putDown
	case(InteractableObjectState::PutDown):
		m_CurrentState = InteractableObjectState::PickedUp;
		pInteractionUser->AddObjectToHand(this);
		break;
		// move into pickedUp
	case(InteractableObjectState::PickedUp):
		pInteractionUser->RemoveCurrentObjectInHand();
		m_CurrentState = InteractableObjectState::PutDown;
		break;
	}
	// when the animation finishes, we want to detach/ attach to the actor that picked us up, i guess.
}


bool AInteractableObject::UpdateDropActionAvailability() const
{
	
}


bool AInteractableObject::IsInteractionAvailable(const UInteractionUserComponent* pInteractionUser) const
{
	if (!pInteractionUser->IsHandFull() && m_CurrentState != InteractableObjectState::PickedUp)
		return true;

	if (pInteractionUser->IsHandFull() && pInteractionUser->GetCurrentInObjectInHand() != this && TryUpdateDroppableInteracton())
	{
		return true;
	}
	return false;
}



