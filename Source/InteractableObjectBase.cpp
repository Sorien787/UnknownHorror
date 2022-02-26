
#include "InteractableObjectBase.h"

#include "UnrealUtilities.h"

AInteractableObjectBase::AInteractableObjectBase()
{
	m_pRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
	check(m_pRootComponent != nullptr);
	RootComponent = m_pRootComponent;
	
	m_pWidgetAttachment = CreateDefaultSubobject<USceneComponent>(TEXT("Widget Attachment Object"));
	check(m_pWidgetAttachment != nullptr);
	m_pWidgetAttachment->SetupAttachment(RootComponent);
	
	m_pInteractWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Interaction Widget"));
	check(m_pInteractWidget != nullptr);
	m_pInteractWidget->SetupAttachment(m_pWidgetAttachment);
}

void AInteractableObjectBase::Tick(float DeltaSeconds)
{
	m_pInteractWidget->SetRelativeRotation(UnrealUtilities::GetRotationMatrixToPlayer(GetWorld(), GetCurrentLocation()));
}


int AInteractableObjectBase::GetInteractionPriority()
{
	return m_InteractionPriority;
}

FVector AInteractableObjectBase::GetCurrentLocation() const
{
	return GetActorLocation();
}

