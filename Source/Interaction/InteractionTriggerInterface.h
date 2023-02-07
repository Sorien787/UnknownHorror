// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractableInterface.h"
#include "InteractionComponentInterface.h"
#include "InteractionTriggerInterface.generated.h"


// This class does not need to be modified.
class UInteractionUserComponent;
UINTERFACE(MinimalAPI)
class UInteractionTriggerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class DEEPSEAHORROR_API IInteractionTriggerInterface : public IItemCon
{
	GENERATED_BODY()
protected:
		
	IInteractableInterface* m_pInteractableInterface{nullptr};

	bool m_bIsCurrentlyActive{true};

public:
	
	bool GetHasLinkedInteractable() const {return m_pInteractableInterface != nullptr;}

	bool GetIsEnabled() const {return m_bIsCurrentlyActive; }
	
	float GetCameraYawTolerance() const{return m_pInteractableInterface->GetCameraYawTolerance();}

	float GetCameraPitchTolerance() const {return m_pInteractableInterface->GetCameraPitchTolerance();}

	virtual bool GetIsFocusable() const	{return false;}

	virtual bool GetIsForcedFocus() const {return false;}

	void SetIsEnabled(bool enabled) { m_bIsCurrentlyActive = enabled;}
	
	virtual void SetForceFocus(bool set){}

	virtual FTransform GetInteractorTransform() const = 0;

	virtual bool GetActorTriggerConditionMet(FTransform actorTransform, FVector actorVelocity) const = 0;
	
	bool GetCanInteract(IInteractionComponentInterface* pUser);

	virtual int GetInteractorId() const = 0;
	
	virtual void TryRevealWidget(){}
	
	virtual void TryHideWidget(){}
	
	virtual void TryFocusWidget(){}
	
	virtual void TryUnfocusWidget(){}

	virtual void TryInteract(IInteractionComponentInterface* pUser){}

	void RegisterParent(IInteractableInterface* pInteractableInterface, bool shouldBeEnabled) {m_pInteractableInterface = pInteractableInterface; SetIsEnabled(shouldBeEnabled);}
	
};
