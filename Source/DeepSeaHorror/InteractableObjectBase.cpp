
#include "InteractableObjectBase.h"

AInteractableObjectBase::AInteractableObjectBase()
{
	m_pInteractWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Interaction Widget"));
	check(m_pInteractWidget != nullptr);
	m_pInteractWidget->SetupAttachment(RootComponent);
}

void AInteractableObjectBase::OnShowInteractionWidget()
{
	RevealWidget();
}

void AInteractableObjectBase::OnHideInteractionWidget()
{
	HideWidget();
}

void AInteractableObjectBase::OnShowFocusedInteractionWidget()
{
	ShowInteraction();
}

void AInteractableObjectBase::OnHideFocusedInteractionWidget()
{
	HideInteraction();
}


int AInteractableObjectBase::GetInteractionPriority()
{
	return m_InteractionPriority;
}

FVector AInteractableObjectBase::GetCurrentLocation() const
{
	return GetOwner()->GetActorLocation();
}

