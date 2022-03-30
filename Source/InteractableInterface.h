// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractableInterface.generated.h"

class UInteractionUserComponent;
enum InteractableObjectType;

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
	virtual void OnInteractionStarted(UInteractionUserComponent* pInteractionUser, int interactorId) = 0;

	virtual bool IsInteractionAvailable(const UInteractionUserComponent* pInteractionUser) const = 0;

	virtual bool IsCombinedInteractionAvailable(const UInteractionUserComponent* pInteractionUser, InteractableObjectType combinedType) const { return false; }

	virtual void OnInteractionFinished(UInteractionUserComponent* pInteractionUser) = 0;
	
	virtual bool IsFastInteraction() const = 0;

	UFUNCTION(BlueprintImplementableEvent)
	void OnInteractWithInteractorId(int interactorId, UInteractionUserComponent* pInteractionUser);
	
	// UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	// void OnInteractorIdsEnabledSet(TArray<int> ids);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnAnimationFinished();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void DisableInteractors();
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnInteractorIdEnabledSet(int id);
};
