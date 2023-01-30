// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/WidgetComponent.h"
#include "Components/BoxComponent.h"
#include "InteractableInterface.h"
#include "InteractionPointInterface.h"
#include "InteractionTriggerInterface.h"
#include "InteractionPoint.generated.h"

class UInteractionUserComponent;

enum class CurrentWidgetState
{
	Hidden,
	Revealed,
	Interactable
};
	
UCLASS()
class DEEPSEAHORROR_API AInteractionPoint : public AActor, public IInteractionPointInterface, public IInteractionTriggerInterface
{
private:
	GENERATED_BODY()

	CurrentWidgetState m_CurrentWidgetState{CurrentWidgetState::Hidden};
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Sets default values for this actor's properties
	AInteractionPoint();
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	virtual bool GetIsFocusable() const override {return true;}

	virtual bool GetIsForcedFocus() const override {return m_bIsForcedFocused;}

	virtual int GetInteractorId() const override {return m_interactorId;}
	
	virtual FTransform GetInteractorTransform() const override {return GetActorTransform();}

	virtual bool GetActorTriggerConditionMet(FTransform actorTransform, FVector actorVelocity) const override;
	
	virtual void SetForceFocus(bool set) override { m_bIsForcedFocused = set; }
	
	virtual void TryRevealWidget() override;
	
	virtual void TryHideWidget() override;
	
	virtual void TryFocusWidget() override;
	
	virtual void TryUnfocusWidget() override;

	virtual void TryInteract(IInteractionComponentInterface* pUser) override;

	bool m_bIsForcedFocused{false};
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	UWidgetComponent* m_pInteractWidget;

	UPROPERTY(EditAnywhere, Category = "Components")
	UBoxComponent* m_InitialTriggerBoxComponent;

	UPROPERTY(EditAnywhere, Category = "Components")
	USceneComponent* m_RootComponent;
	
	UPROPERTY(EditAnywhere, Category = "Interaction Settings")
	int m_interactorId{0};
	
	UPROPERTY(EditAnywhere, Category = "Components")
	USceneComponent* m_WidgetAttachmentPoint;

	UPROPERTY(EditAnywhere, Category = "Interaction Settings",  meta = (ClampMin = "0.0", ClampMax = "180.0", UIMin = "0.0", UIMax = "180.0"))
	float m_TriggerAngle{90};
};
