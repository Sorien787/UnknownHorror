
#include "InteractableObjectBase.h"
#include "InteractionUserComponent.h"
#include "InteractionPoint.h"

AInteractableObjectBase::AInteractableObjectBase()
{
	m_pRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
	check(m_pRootComponent != nullptr);
	RootComponent = m_pRootComponent;
}

void AInteractableObjectBase::BeginPlay()
{
	Super::BeginPlay();

	TInlineComponentArray<AInteractionPoint*> pInteractionPoints;
	TArray<AActor*> pActorChildren;
	GetAttachedActors(pActorChildren);
	for (int i = 0; i < pActorChildren.Num(); i++)
	{
		AActor* pActor = pActorChildren[i];
		AInteractionPoint* pInteractionPoint = Cast<AInteractionPoint>(pActor);
		if (!pInteractionPoint)
			continue;

		m_pInteractionPoints.Add(pInteractionPoint);
		pInteractionPoint->RegisterParent(this);
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

void AInteractableObjectBase::OnInteractionStarted(UInteractionUserComponent* pInteractionUser, int interactorId)
{
	m_pCurrentUser = pInteractionUser;

	
}

bool AInteractableObjectBase::IsFastInteraction() const
{
	return m_bIsFastInteraction;
}

void AInteractableObjectBase::OnInteractorIdEnabledSet(int id, bool enabled)
{
	for (int i = 0; i < m_pInteractionPoints.Num(); i++)
	{
		if (m_pInteractionPoints[i]->GetInteractorId() != id)
			continue;
		
		m_pInteractionPoints[i]->SetIsEnabled(enabled);
		return;
	}
}



