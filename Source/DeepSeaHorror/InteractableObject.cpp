// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractableObject.h"

#include "UnrealUtilities.h"


void AInteractableObject::BeginPlay()
{
	Super::BeginPlay();
}

void AInteractableObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AInteractableObject::OnInteractionStarted(UInteractionUserComponent* pInteractionUser)
{
	
}

// we could do a sort of interaction animation callback?
// so, for this one, we get a callback when the hand IK touches, and attach it to the hand
// for doors, we get a callback when the hand touches the door, then the door opens and the hand IK follows while the skeleton

// for pickup/put down:
// stage 1: start anim 
void AInteractableObject::OnInteractionFinished(UInteractionUserComponent* pInteractionUser)
{
	// switch(m_CurrentState)
	// {
	// 	// move into putDown
	// case(InteractableObjectState::PickedUp):
	// 	m_CurrentState = InteractableObjectState::PutDown;
	// 	pInteractionUser->AttachObjectToHand();
	// 	break;
	// 	// move into pickedUp
	// case(InteractableObjectState::PutDown):
	// 	pInteractionUser->DetachInteractableObject();
	// 	m_CurrentState = InteractableObjectState::PickedUp;
	// 	break;
	// }
	// when the animation finishes, we want to detach/ attach to the actor that picked us up, i guess.
}

bool AInteractableObject::IsInteractionAvailable(const UInteractionUserComponent* pInteractionUser, OUT int type) const
{
	// interaction is only available
	if (m_CurrentState == InteractableObjectState::PickedUp)
	{
		type = 0;
		const FHitResult hit = UnrealUtilities::RaycastActorToWorldHit(GetWorld(), pInteractionUser->GetDropRange(), GetWorld()->GetFirstPlayerController()->GetOwner());
		return hit.GetActor();
	}
	else
	{
		type = 1;
		return true;
	}
}

int AInteractableObject::GetInteractionPriority()
{
	return m_InteractionPriority;
}


void AInteractableObject::OnShowInteractionWidget()
{
	
}


void AInteractableObject::OnHideInteractionWidget()
{
	
}


FVector AInteractableObject::GetCurrentLocation() const
{
	return GetOwner()->GetActorLocation();
}

