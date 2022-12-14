// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractableInterface.h"
#include "InteractionPoint.h"
#include "InteractionTriggerInterface.h"
#include "Components/WidgetComponent.h"
#include "InteractableObjectBase.generated.h"

UCLASS(Abstract)
class DEEPSEAHORROR_API AInteractableObjectBase : public AActor, public IInteractableInterface
{
	GENERATED_BODY()
private:


public:	
	AInteractableObjectBase();

	virtual void BeginPlay() override;
	// no implementation in the base
	virtual bool IsInteractionAvailable(const UInteractionUserComponent* pInteractionUser, int interactorId) override;

	virtual void OnInteractionFinished(UInteractionUserComponent* pInteractionUser) override;

	virtual void OnInteractionStarted(UInteractionUserComponent* pInteractionUser, FVector pointRelativePosition, FQuat pointRelativeRotation, int interactorId) override;
	
	virtual bool IsFastInteraction() const override;

	virtual float GetCameraYawTolerance() const override;

	virtual float GetCameraPitchTolerance() const override;

	UPROPERTY(EditAnywhere, Category = "Components")
		USceneComponent* m_pRootComponent;
	
	UPROPERTY(EditAnywhere, Category = "Components")
		bool m_bIsFastInteraction;

	UPROPERTY(EditAnywhere, Category = "Components")
		float m_DefaultCameraYawTolerance = 20.0f;

	UPROPERTY(EditAnywhere, Category = "Components")
		float m_DefaultCameraPitchTolerance = 20.0f;
	
	void OnAnimationFinished_Implementation() override;

	void DisableInteractors_Implementation() override;

	void OnInteractorIdEnabledSet_Implementation(int id) override;

	virtual FTransform GetInteractionPointTransform_Implementation(const int interactorId) override;

	virtual FTransform GetDesiredTransformForInteraction_Implementation(const int interactorId, const UInteractionUserComponent* pInteractionUser) override;
	
	IInteractionTriggerInterface* FindInteractionPointById(int id);

	// void OnInteractorIdsEnabledSet_Implementation(TArray<int> ids) override;
	
	UInteractionUserComponent* m_pCurrentUser {nullptr};

	TArray<IInteractionTriggerInterface*> m_pInteractionPoints;

	TSet<int> m_EnabledInteractionPoints;
};
