// Fill out your copyright notice in the Description page of Project Settings.


#include "DualStateInteractable.h"

// Sets default values
ADualStateInteractable::ADualStateInteractable()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void ADualStateInteractable::BeginPlay()
{
	Super::BeginPlay();
	
}

void ADualStateInteractable::OnInteractionStarted(UInteractionUserComponent* pInteractionUser)
{
	// for now, immediately finish the interaction - we'll sort out the proper animation delay later.
	OnInteractionFinished(pInteractionUser);
}

bool ADualStateInteractable::IsInteractionAvailable(const UInteractionUserComponent* pInteractionUser, int type) const
{
	// get interaction user type here?
	
	return true;
}

void ADualStateInteractable::OnInteractionFinished(UInteractionUserComponent* pInteractionUser)
{
	if (m_CurrentState == DualStateObjectState::Closed)
		m_CurrentState = DualStateObjectState::Open;
	else
		m_CurrentState = DualStateObjectState::Closed;
	OnChangeState(m_CurrentState);
}

void ADualStateInteractable::OnShowInteractionWidget()
{
	
}

int ADualStateInteractable::GetInteractionPriority()
{
	return 0;
}

void ADualStateInteractable::OnHideInteractionWidget()
{
}

FVector ADualStateInteractable::GetCurrentLocation() const
{
	return GetOwner()->GetActorLocation();
}



