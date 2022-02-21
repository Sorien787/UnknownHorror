// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractionUserComponent.h"
#include "UObject/Interface.h"
#include "InteractableInterface.generated.h"

class UInteractionUserComponent;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInteractableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class DEEPSEAHORROR_API IInteractableInterface
{	
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void OnInteractionStarted(UInteractionUserComponent* pInteractionUser) = 0;

	virtual bool IsInteractionAvailable(const UInteractionUserComponent* pInteractionUser, OUT int type) const = 0;

	virtual bool IsCombinedInteractionAvailable(const UInteractionUserComponent* pInteractionUser, InteractableObjectType combinedType) const { return false; }

	virtual void OnInteractionFinished(UInteractionUserComponent* pInteractionUser) = 0;

	virtual void OnShowInteractionWidget() = 0;

	virtual int GetInteractionPriority() = 0;

	virtual void OnShowFocusedInteractionWidget() = 0;

	virtual void OnHideFocusedInteractionWidget() = 0;

	virtual void OnHideInteractionWidget() = 0;

	virtual FVector GetCurrentLocation() const = 0;
};
