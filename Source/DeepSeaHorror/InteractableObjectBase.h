// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractableInterface.h"
#include "Components/WidgetComponent.h"
#include "InteractableObjectBase.generated.h"

UCLASS(Abstract)
class DEEPSEAHORROR_API AInteractableObjectBase : public AActor, public IInteractableInterface
{
	GENERATED_BODY()
	
public:	
	AInteractableObjectBase();
	
	virtual void OnShowInteractionWidget() override;

	virtual void OnHideInteractionWidget() override;

	virtual void OnShowFocusedInteractionWidget() override;

	virtual void OnHideFocusedInteractionWidget() override;

	virtual int GetInteractionPriority() override;

	virtual FVector GetCurrentLocation() const override;

	UPROPERTY(EditAnywhere)
		UWidgetComponent* m_pInteractWidget;

	UPROPERTY(EditAnywhere)
		int m_InteractionPriority;
	
	UFUNCTION(BlueprintImplementableEvent)
		void RevealWidget();

	UFUNCTION(BlueprintImplementableEvent)
		void HideWidget();

	UFUNCTION(BlueprintImplementableEvent)
		void ShowInteraction();

	UFUNCTION(BlueprintImplementableEvent)
		void HideInteraction();
};
