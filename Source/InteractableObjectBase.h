// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractableInterface.h"
#include "InteractionPoint.h"
#include "Components/BoxComponent.h"
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
	virtual bool IsInteractionAvailable(const UInteractionUserComponent* pInteractionUser, int type) const override;

	virtual void OnInteractionFinished(UInteractionUserComponent* pInteractionUser) override;

	virtual void OnInteractionStarted(UInteractionUserComponent* pInteractionUser, int interactorId) override;

	virtual bool IsFastInteraction() const override;
	
	UPROPERTY(EditAnywhere, Category = "Components")
		USceneComponent* m_pWidgetAttachment;

	UPROPERTY(EditAnywhere, Category = "Components")
		USceneComponent* m_pRootComponent;
	
	UPROPERTY(EditAnywhere, Category = "Components")
		bool m_bIsFastInteraction;

	void OnAnimationFinished_Implementation() override;

	void DisableInteractors_Implementation() override;

	void OnInteractorIdEnabledSet_Implementation(int id) override;

	// void OnInteractorIdsEnabledSet_Implementation(TArray<int> ids) override;
	
	UInteractionUserComponent* m_pCurrentUser {nullptr};

	TArray<AInteractionPoint*> m_pInteractionPoints;

	TSet<int> m_EnabledInteractionPoints;
};
