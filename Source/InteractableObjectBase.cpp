
#include "InteractableObjectBase.h"

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
	m_pInteractWidget->GetWidget()->PlayAnimation(m_RevealWidgetAnimation);
}

void AInteractableObjectBase::TryHideWidget()
{
	if (m_CurrentWidgetState == CurrentWidgetState::Hidden)
		return;
	m_pInteractWidget->GetWidget()->PlayAnimation(m_HideWidgetAnimation);
}

void AInteractableObjectBase::TryFocusWidget()
{
	if (m_CurrentWidgetState == CurrentWidgetState::Interactable)
		return;
	m_pInteractWidget->GetWidget()->PlayAnimation(m_FocusWidgetAnimation);
}

void AInteractableObjectBase::TryUnfocusWidget()
{
	if (m_CurrentWidgetState != CurrentWidgetState::Interactable)
		return;
	m_pInteractWidget->GetWidget()->PlayAnimation(m_UnfocusWidgetAnimation);
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
		m_pInteractWidget->GetWidget()->PlayAnimation(m_InteractFastWidgetAnimation);
		return;
	}
	m_pInteractWidget->GetWidget()->PlayAnimation(m_InteractSlowWidgetAnimation);
	m_CurrentWidgetState = CurrentWidgetState::Hidden;
}


