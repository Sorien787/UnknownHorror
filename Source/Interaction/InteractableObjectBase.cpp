
#include "InteractableObjectBase.h"

#include "InteractionAlignmentComponent.h"
#include "InteractionUserComponent.h"
#include "InteractionPoint.h"
#include "Common/UnrealUtilities.h"

AInteractableObjectBase::AInteractableObjectBase()
{
	m_pRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
	check(m_pRootComponent != nullptr);
	RootComponent = m_pRootComponent;
}

void AInteractableObjectBase::BeginPlay()
{
	Super::BeginPlay();
	
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

	TArray<UInteractionAlignmentComponent*> components;
	GetComponents<UInteractionAlignmentComponent>(components);
	
	for (int i = 0; i < components.Num(); i++)
    {
    	m_pInteractionAlignmentPoints.Add(components[i]);
    }
}

void AInteractableObjectBase::OnInteractionFinished(const TScriptInterface<IInteractionComponentInterface>& pInteractionUser)
{
	if (!m_pCurrentUser)
		return;
	m_pCurrentUser->OnInteractionFinished(this);
	m_pCurrentUser = nullptr;
}

void AInteractableObjectBase::OnInteractionStarted(const TScriptInterface<IInteractionComponentInterface>& pInteractionUser, FVector pointRelativePosition, FQuat pointRelativeRotation, int interactorId)
{
	bool result;
	Execute_OnInteractWithInteractorId(this, interactorId, pInteractionUser, result);
}

FTransform AInteractableObjectBase::GetInteractionPointTransform_Implementation(const int interactorId)
{
	for (int i = 0; i < m_pInteractionPoints.Num(); i++)
	{
		if (m_pInteractionPoints[i]->GetInteractorId() != interactorId)
			continue;
		
		return m_pInteractionPoints[i]->GetInteractorTransform();
	}

	for (int i = 0; i < m_pInteractionAlignmentPoints.Num(); i++)
	{
		if (m_pInteractionAlignmentPoints[i]->GetInteractorId() != interactorId)
			continue;
		return m_pInteractionAlignmentPoints[i]->GetComponentTransform();
	}
	return GetActorTransform();
}

FTransform AInteractableObjectBase::GetDesiredTransformForInteraction_Implementation(const int interactorId, const InteractionUserType pInteractionUser)
{
	return GetInteractionPointTransform(interactorId);
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
	if (!m_pCurrentUser)
		return;
	m_pCurrentUser->OnInteractionFinished(this);
	m_pCurrentUser = nullptr;
}

void AInteractableObjectBase::OnInteractWithInteractorId_Implementation(const int interactorId, const TScriptInterface<IInteractionComponentInterface>& pInteractionUser, bool& returnResult)
{
	m_pCurrentUser = pInteractionUser;
	m_pCurrentUser->OnInteractionStarted(this);
}

FVector AInteractableObjectBase::GetInteractableLocation_Implementation() const
{
	return GetActorLocation();
}

void AInteractableObjectBase::GetPossibleAvailableInteractions_Implementation(const InteractionUserType pInteractionUser, TArray<int>& result)
{
	for (size_t nInteractionPointIndex = 0; nInteractionPointIndex < m_pInteractionPoints.Num(); nInteractionPointIndex++)
	{
		bool res = false;
		int interactorID = m_pInteractionPoints[nInteractionPointIndex]->GetInteractorId();
		Execute_IsInteractionAvailable(this, interactorID, pInteractionUser,res);
		if (!res)
			continue;
		result.Push(interactorID);
	}
	for (size_t nInteractionAlignmentPoint = 0; nInteractionAlignmentPoint < m_pInteractionAlignmentPoints.Num(); nInteractionAlignmentPoint++)
	{
		bool res = false;
		int interactorID = m_pInteractionAlignmentPoints[nInteractionAlignmentPoint]->GetInteractorId();
		Execute_IsInteractionAvailable(this, interactorID, pInteractionUser,res);
		if (!res)
			continue;
		result.Push(interactorID);
	}
}

void AInteractableObjectBase::DisableInteractors_Implementation()
{
	m_EnabledInteractionPoints.Empty();
	for (int i = 0; i < m_pInteractionPoints.Num(); i++)
	{
		m_pInteractionPoints[i]->SetIsEnabled(false);		
	}
}





