
#include "InteractableObjectBase.h"
#pragma optimize("", off)
#include "UnrealUtilities.h"

AInteractableObjectBase::AInteractableObjectBase()
{
	m_pRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
	check(m_pRootComponent != nullptr);
	RootComponent = m_pRootComponent;

	m_pInteractWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Interaction Widget"));
	check(m_pInteractWidget != nullptr);
	m_pInteractWidget->SetupAttachment(GetRootComponent());

	m_TriggerBoxComponent= CreateDefaultSubobject<UBoxComponent>(TEXT("Interaction Trigger Box"));
	check(m_TriggerBoxComponent != nullptr);
	m_TriggerBoxComponent->SetupAttachment(GetRootComponent());
}

void AInteractableObjectBase::TryRevealWidget()
{
	if (m_CurrentWidgetState != CurrentWidgetState::Hidden)
		return;
	m_CurrentWidgetState = CurrentWidgetState::Revealed;
	Execute_InteractionWidgetReveal(this);
}

void AInteractableObjectBase::TryHideWidget()
{
	if (m_CurrentWidgetState == CurrentWidgetState::Hidden)
		return;
	m_CurrentWidgetState = CurrentWidgetState::Hidden;
	Execute_InteractionWidgetHide(this);
}

void AInteractableObjectBase::TryFocusWidget()
{
	if (m_CurrentWidgetState == CurrentWidgetState::Interactable)
		return;
	m_CurrentWidgetState = CurrentWidgetState::Interactable;
	Execute_InteractionWidgetFocus(this);
}

void AInteractableObjectBase::TryUnfocusWidget()
{
	if (m_CurrentWidgetState != CurrentWidgetState::Interactable)
		return;
	m_CurrentWidgetState = CurrentWidgetState::Revealed;
	Execute_InteractionWidgetUnfocus(this);
}

FVector AInteractableObjectBase::GetCurrentLocation() const
{
	return GetActorLocation();
}

void AInteractableObjectBase::Tick(float DeltaSeconds)
{
	m_pInteractWidget->SetWorldRotation(UnrealUtilities::GetRotationMatrixToPlayer(GetWorld(), GetCurrentLocation()));
}

void AInteractableObjectBase::OnInteractionStarted(UInteractionUserComponent* pInteractionUser)
{
	if (m_bIsQuickInteraction)
	{
		Execute_InteractionWidgetInteractFast(this);
		return;
	}
	Execute_InteractionWidgetInteractSlow(this);
	m_CurrentWidgetState = CurrentWidgetState::Hidden;
}


