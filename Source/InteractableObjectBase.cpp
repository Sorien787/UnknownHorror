
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
		bool shouldEnable = m_EnabledInteractionPoints.Contains(pInteractionPoint->GetInteractorId());
		pInteractionPoint->RegisterParent(this, shouldEnable);
	}
}

bool AInteractableObjectBase::IsInteractionAvailable(const UInteractionUserComponent* pInteractionUser, int interactorId)
{
	bool isAvailable = true;
	Execute_IsInteractionAvailableOverride(this, interactorId, pInteractionUser, isAvailable);
	return m_pCurrentUser == nullptr && isAvailable;
}

void AInteractableObjectBase::OnInteractionFinished(UInteractionUserComponent* pInteractionUser)
{
	if (!m_pCurrentUser)
		return;
	m_pCurrentUser->OnInteractionFinished();
	m_pCurrentUser = nullptr;
}

void AInteractableObjectBase::OnInteractionStarted(UInteractionUserComponent* pInteractionUser, int interactorId)
{
	m_pCurrentUser = pInteractionUser;
	bool result;
	Execute_OnInteractWithInteractorId2(this, interactorId, pInteractionUser, result);
}

bool AInteractableObjectBase::IsFastInteraction() const
{
	return m_bIsFastInteraction;
}

float AInteractableObjectBase::GetCameraYawTolerance() const
{
	return m_DefaultCameraYawTolerance;
}

float AInteractableObjectBase::GetCameraPitchTolerance() const
{
	return m_DefaultCameraPitchTolerance;
}

void AInteractableObjectBase::OnInteractorIdEnabledSet_Implementation(int id)
{
	m_EnabledInteractionPoints.Add(id);
	for (int i = 0; i < m_pInteractionPoints.Num(); i++)
	{
		if (m_pInteractionPoints[i]->GetInteractorId() != id)
			continue;
		
		m_pInteractionPoints[i]->SetIsEnabled(true);
		return;
	}
}

IInteractionTriggerInterface* AInteractableObjectBase::FindInteractionPointById(int id)
{
	for (int i = 0; i < m_pInteractionPoints.Num(); i++)
	{
		if (m_pInteractionPoints[i]->GetInteractorId() != id)
			continue;
		return m_pInteractionPoints[i];
	}

	return nullptr;
}

void AInteractableObjectBase::OnAnimationFinished_Implementation()
{
	OnInteractionFinished(m_pCurrentUser);
}


void AInteractableObjectBase::DisableInteractors_Implementation()
{
	m_EnabledInteractionPoints.Empty();
	for (int i = 0; i < m_pInteractionPoints.Num(); i++)
	{
		m_pInteractionPoints[i]->SetIsEnabled(false);		
	}
}





