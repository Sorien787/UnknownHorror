// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractableObject.h"

#include "UnrealUtilities.h"
#include "InteractionUserComponent.h"
#pragma optimize("", off)

void AInteractableObject::BeginPlay()
{
	Super::BeginPlay();
}

AInteractableObject::AInteractableObject()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
}


void AInteractableObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AInteractionPoint* pInteractionPoint = GetDropInteractionPoint();
	
	if (!pInteractionPoint)
		return;

	FTransform cameraTransform = GetWorld()->GetFirstPlayerController()->PlayerCameraManager->GetTransform();

	const float maxAngleCanPlace = 75;

	const float angleFromDownVector = acos(FVector::DotProduct(cameraTransform.GetUnitAxis(EAxis::X), (FVector(0, -1, 0))));

	m_bIsDropActionValid = angleFromDownVector < maxAngleCanPlace;
	return;
	// if (angleFromDownVector > maxAngleCanPlace)
	// {
	// 	pInteractionPoint->SetIsEnabled(false);
	// 	return;
	// }
	//
	//
	//
	// // FHitResult worldHit = UnrealUtilities::RaycastActorToWorldHit(GetWorld(), m_pCurrentUser->GetInteractionRange(), GetWorld()->GetFirstPlayerController());
	// //
	// //
	// //
	// // if (!worldHit.GetActor())
	// // {
	// // 	pInteractionPoint->SetIsEnabled(false);
	// // 	return;
	// // }
	//
	// pInteractionPoint->SetIsEnabled(true);
	// pInteractionPoint->SetActorLocation(worldHit.Location);
}

void AInteractableObject::OnInteractionStarted(UInteractionUserComponent* pInteractionUser, int interactorId)
{
	Super::OnInteractionStarted(pInteractionUser, interactorId);

	switch(m_CurrentState)
	{
	case(InteractableObjectState::PutDown):
		SetActorEnableCollision(false);
		break;
	case(InteractableObjectState::PickedUp):
		GetDropInteractionPoint()->SetForceFocus(false);
		SetActorTickEnabled(false);
		break;
	}
	OnInteractionFinished(pInteractionUser);
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
		SetActorTickEnabled(true);
		GetDropInteractionPoint()->SetForceFocus(true);
		pInteractionUser->AddObjectToHand(this);
		break;
		// move into pickedUp
	case(InteractableObjectState::PickedUp):
		pInteractionUser->RemoveCurrentObjectInHand();
		m_CurrentState = InteractableObjectState::PutDown;
		SetActorEnableCollision(true);
		break;
	}
	// when the animation finishes, we want to detach/ attach to the actor that picked us up, i guess.

	Super::OnInteractionFinished(pInteractionUser);
}


bool AInteractableObject::IsDropActionValid() const
{
	return m_bIsDropActionValid;
}


bool AInteractableObject::IsInteractionAvailable(const UInteractionUserComponent* pInteractionUser, int interactorId)
{
	if (!pInteractionUser->IsHandFull() && m_CurrentState != InteractableObjectState::PickedUp)
		return true;

	if (pInteractionUser->IsHandFull() && pInteractionUser->GetCurrentInObjectInHand() == this && IsDropActionValid())
	{
		return true;
	}
	return false;
}

AInteractionPoint* AInteractableObject::GetDropInteractionPoint()
{
	return FindInteractionPointById(m_nDropInteractionPointId);
}



