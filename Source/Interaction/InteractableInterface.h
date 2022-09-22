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

public:
	virtual void OnInteractionStarted(UInteractionUserComponent* pInteractionUser, FVector pointRelativePosition, FQuat pointRelativeRotation, int interactorId) = 0;

	virtual bool IsInteractionAvailable(const UInteractionUserComponent* pInteractionUser, int interactorId)  = 0;

	virtual bool IsCombinedInteractionAvailable(const UInteractionUserComponent* pInteractionUser, InteractableObjectType combinedType) { return false; }

	virtual void OnInteractionFinished(UInteractionUserComponent* pInteractionUser) = 0;
	
	virtual bool IsFastInteraction() const = 0;

	virtual float GetCameraYawTolerance() const = 0;

	virtual float GetCameraPitchTolerance() const = 0;

	UFUNCTION(BlueprintNativeEvent)
	void IsInteractionAvailableOverride(const int interactorId, const UInteractionUserComponent* pInteractionUser, bool& returnResult);//const UInteractionUserComponent* pInteractionUser, const int interactorId);

	void IsInteractionAvailableOverride_Implementation(const int interactorId, const UInteractionUserComponent* pInteractionUser,  bool& returnResult);//const UInteractionUserComponent* pInteractionUser, const int interactorId);

	UFUNCTION(BlueprintNativeEvent)
	void OnInteractWithInteractorId2(const int interactorId, const UInteractionUserComponent* pInteractionUser, FVector pointRelativePosition, FQuat pointRelativeRotation, bool& returnResult);

	void OnInteractWithInteractorId2_Implementation(const int interactorId, const UInteractionUserComponent* pInteractionUser, FVector pointRelativePosition, FQuat pointRelativeRotation, bool& returnResult);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnAnimationFinished();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void DisableInteractors();
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnInteractorIdEnabledSet(int id);
};
