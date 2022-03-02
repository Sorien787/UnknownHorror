// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractableInterface.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "InteractableObjectBase.generated.h"

enum class CurrentWidgetState
{
	Hidden,
	Revealed,
	Interactable
};
	
UCLASS(Abstract)
class DEEPSEAHORROR_API AInteractableObjectBase : public AActor, public IInteractableInterface
{
	GENERATED_BODY()
private:

	CurrentWidgetState m_CurrentWidgetState{CurrentWidgetState::Hidden};
public:	
	AInteractableObjectBase();

	virtual void TryRevealWidget() override;
	
	virtual void TryHideWidget() override;
	
	virtual void TryFocusWidget() override;
	
	virtual void TryUnfocusWidget() override;

	virtual FVector GetCurrentLocation() const override;

	virtual void Tick(float DeltaSeconds) override;

	// no implementation in the base
	virtual bool IsInteractionAvailable(const UInteractionUserComponent* pInteractionUser, int type) const override {return false; }

	virtual void OnInteractionFinished(UInteractionUserComponent* pInteractionUser) override {}

	virtual void OnInteractionStarted(UInteractionUserComponent* pInteractionUser) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
		UWidgetComponent* m_pInteractWidget;

	UPROPERTY(EditAnywhere, Category = "Components")
		USceneComponent* m_pWidgetAttachment;

	UPROPERTY(EditAnywhere, Category = "Components")
		USceneComponent* m_pRootComponent;

	UPROPERTY(EditAnywhere, Category = "Components")
	    UBoxComponent* m_TriggerBoxComponent;

	UPROPERTY(EditAnywhere, Category = "Interaction Settings")
		bool m_bIsQuickInteraction{true};

	UPROPERTY(EditAnywhere, Category = "Interaction Settings")
		int m_InteractionPriority;

	UPROPERTY(Transient, EditAnywhere, Category = "Widget Anims", meta = (BindWidgetAnim))
		UWidgetAnimation* m_RevealWidgetAnimation;

	UPROPERTY(EditAnywhere, Category = "Widget Anims", meta = (BindWidgetAnim))
		UWidgetAnimation* m_HideWidgetAnimation;

	UPROPERTY(EditAnywhere, Category = "Widget Anims", meta = (BindWidgetAnim))
		UWidgetAnimation* m_FocusWidgetAnimation;

	UPROPERTY(EditAnywhere, Category = "Widget Anims", meta = (BindWidgetAnim))
		UWidgetAnimation* m_UnfocusWidgetAnimation;

	UPROPERTY(EditAnywhere, Category = "Widget Anims", meta = (BindWidgetAnim))
		UWidgetAnimation* m_InteractFastWidgetAnimation;

	UPROPERTY(EditAnywhere, Category = "Widget Anims", meta = (BindWidgetAnim))
		UWidgetAnimation* m_InteractSlowWidgetAnimation;

};
