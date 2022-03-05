
#include "InteractableObjectBase.h"
#include "InteractionUserComponent.h"
#include "InteractionPoint.h"

AInteractableObjectBase::AInteractableObjectBase()
{
	m_pRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
	check(m_pRootComponent != nullptr);
	RootComponent = m_pRootComponent;
	
	TInlineComponentArray<AInteractionPoint*> pInteractionPoints;
	GetComponents(pInteractionPoints);
	for (auto it = pInteractionPoints.CreateIterator(); it; ++it)
	{
		(*it)->RegisterParent(this);
	}
}

bool AInteractableObjectBase::IsInteractionAvailable(const UInteractionUserComponent* pInteractionUser, int type) const
{
	return m_pCurrentUser == nullptr;
}

void AInteractableObjectBase::OnInteractionFinished(UInteractionUserComponent* pInteractionUser)
{
	m_pCurrentUser->OnInteractionFinished();
	m_pCurrentUser = nullptr;
}

void AInteractableObjectBase::OnInteractionStarted(UInteractionUserComponent* pInteractionUser)
{
	m_pCurrentUser = pInteractionUser;
}

bool AInteractableObjectBase::IsFastInteraction() const
{
	return m_bIsFastInteraction;
}



