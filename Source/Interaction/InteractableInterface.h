// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractionComponentInterface.h"
#include "UObject/Interface.h"
#include "InteractableInterface.generated.h"

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

public:
	virtual void OnInteractionStarted(const TScriptInterface<IInteractionComponentInterface>& pInteractionUser, FVector pointRelativePosition, FQuat pointRelativeRotation, int interactorId) = 0;
	
	virtual void OnInteractionFinished(const TScriptInterface<IInteractionComponentInterface>& pInteractionUser) = 0;

	virtual float GetCameraYawTolerance() const = 0;

	virtual float GetCameraPitchTolerance() const = 0;

	virtual UObject* GetThisObject() = 0;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	FVector GetInteractableLocation() const;
	virtual FVector GetInteractableLocation_Implementation() const;
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void IsInteractionAvailable(const int interactorId, const TScriptInterface<IInteractionComponentInterface>& InteractionUser, bool& returnResult);//const UInteractionUserComponent* pInteractionUser, const int interactorId);
	virtual void IsInteractionAvailable_Implementation(const int interactorId, const TScriptInterface<IInteractionComponentInterface>& InteractionUser,  bool& returnResult);//const UInteractionUserComponent* pInteractionUser, const int interactorId);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnInteractWithUsingInteractable(const int interactorId, const TScriptInterface<IInteractionComponentInterface>& pInteractionUser);
	virtual void OnInteractWithUsingInteractable_Implementation(const int interactorId, const TScriptInterface<IInteractionComponentInterface>& pInteractionUser);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void GetPossibleAvailableInteractions(const TScriptInterface<IInteractionComponentInterface>& pInteractionUser, TArray<int>& result);
	virtual void GetPossibleAvailableInteractions_Implementation(const TScriptInterface<IInteractionComponentInterface>& pInteractionUser, TArray<int>& result);
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnInteractWithInteractorId(const int interactorId, const TScriptInterface<IInteractionComponentInterface>& pInteractionUser, bool& returnResult);
	virtual void OnInteractWithInteractorId_Implementation(const int interactorId, const TScriptInterface<IInteractionComponentInterface>& pInteractionUser, bool& returnResult);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	FTransform GetDesiredTransformForInteraction(const int interactorId, const InteractionUserType pInteractionUser);
	virtual FTransform GetDesiredTransformForInteraction_Implementation(const int interactorId, const InteractionUserType pInteractionUser);
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	FTransform GetInteractionPointTransform(const int interactorId);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool CheckItemControlPriority(const AActor* requester) const;
	virtual bool CheckItemControlPriority_Implementation(const AActor* requester) const {return false;}
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnAnimationFinished();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void DisableInteractors();
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnInteractorIdEnabledSet(int id);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool TryCancelInteraction(const TScriptInterface<IInteractionComponentInterface>& pInteractionUser);
	virtual bool TryCancelInteraction_Implementation(const TScriptInterface<IInteractionComponentInterface>& pInteractionUser);
};
