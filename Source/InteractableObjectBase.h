// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractableInterface.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "InteractableObjectBase.generated.h"

UCLASS(Abstract)
class DEEPSEAHORROR_API AInteractableObjectBase : public AActor, public IInteractableInterface
{
	GENERATED_BODY()
public:	
	AInteractableObjectBase();

	virtual int GetInteractionPriority() override;

	virtual FVector GetCurrentLocation() const override;

	virtual void Tick(float DeltaSeconds) override;

	// no implementation in the base
	virtual bool IsInteractionAvailable(const UInteractionUserComponent* pInteractionUser, int type) const override {return false; }

	virtual void OnInteractionFinished(UInteractionUserComponent* pInteractionUser) override {}

	virtual void OnInteractionStarted(UInteractionUserComponent* pInteractionUser) override
	{
		if (m_bIsQuickInteraction)
			Execute_OnInstantInteract(this);
		else
			Execute_OnAnimInteract(this);
	}
	// no implementation in the base

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		UWidgetComponent* m_pInteractWidget;

	UPROPERTY(EditAnywhere)
		USceneComponent* m_pWidgetAttachment;

	UPROPERTY(EditAnywhere)
		USceneComponent* m_pRootComponent;

	UPROPERTY(EditAnywhere)
	    UBoxComponent* m_TriggerBoxComponent;

	UPROPERTY(EditAnywhere)
		bool m_bIsQuickInteraction{true};

	UPROPERTY(EditAnywhere)
		int m_InteractionPriority;
};
