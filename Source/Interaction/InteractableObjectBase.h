// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractableInterface.h"
#include "InteractionTriggerInterface.h"
#include "Components/WidgetComponent.h"
#include "InteractableObjectBase.generated.h"

class UInteractionAlignmentComponent;

UCLASS(Abstract)
class DEEPSEAHORROR_API AInteractableObjectBase : public AActor, public IInteractableInterface
{
	GENERATED_BODY()
private:


public:	
	AInteractableObjectBase();

	virtual void BeginPlay() override;
	// no implementation in the base
	virtual void OnInteractionFinished(const TScriptInterface<IInteractionComponentInterface>& pInteractionUser) override;

	virtual void OnInteractionStarted(const TScriptInterface<IInteractionComponentInterface>& pInteractionUser, FVector pointRelativePosition, FQuat pointRelativeRotation, int interactorId) override;
	
	virtual FTransform GetInteractionPointTransform_Implementation(const int interactorId) override;

	virtual FTransform GetDesiredTransformForInteraction_Implementation(const int interactorId, const InteractionUserType pInteractionUser) override;

	virtual void OnInteractWithInteractorId_Implementation(const int interactorId, const TScriptInterface<IInteractionComponentInterface>& pInteractionUser, bool& returnResult) override;

	virtual void IsInteractionAvailable_Implementation(const int interactorId, const TScriptInterface<IInteractionComponentInterface>& InteractionUser,  bool& returnResult) override;//const UInteractionUserComponent* pInteractionUser, const int interactorId);

	virtual FVector GetInteractableLocation_Implementation() const override;

	virtual void GetPossibleAvailableInteractions_Implementation(const InteractionUserType pInteractionUser, TArray<int>& result) override;
	
	virtual float GetCameraYawTolerance() const override;

	virtual float GetCameraPitchTolerance() const override;

	virtual UObject* GetThisObject() override{return this;}
	
	void OnAnimationFinished_Implementation() override;

	void DisableInteractors_Implementation() override;

	void OnInteractorIdEnabledSet_Implementation(int id) override;
	
	UPROPERTY(EditAnywhere, Category = "Components")
		USceneComponent* m_pRootComponent;

	UPROPERTY(EditAnywhere, Category = "Components")
		UItemControlComponent* m_pItemControlComponent;
	
	UPROPERTY(EditAnywhere, Category = "Implementation Details")
		bool m_bIsFastInteraction;

	UPROPERTY(EditAnywhere, Category = "Implementation Details")
		float m_DefaultCameraYawTolerance = 20.0f;

	UPROPERTY(EditAnywhere, Category = "Implementation Details")
		float m_DefaultCameraPitchTolerance = 20.0f;

	IInteractionTriggerInterface* FindInteractionPointById(int id);

	TScriptInterface<IInteractionComponentInterface> m_pCurrentUser {nullptr};

	TArray<IInteractionTriggerInterface*> m_pInteractionPoints;
	
	TArray<UInteractionAlignmentComponent*> m_pInteractionAlignmentPoints;
	
	TSet<int> m_EnabledInteractionPoints;
};
