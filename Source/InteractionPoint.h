// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/WidgetComponent.h"
#include "Components/BoxComponent.h"
#include "InteractableInterface.h"
#include "InteractionPointInterface.h"
#include "InteractionPoint.generated.h"

enum class CurrentWidgetState
{
	Hidden,
	Revealed,
	Interactable
};
	
UCLASS()
class DEEPSEAHORROR_API AInteractionPoint : public AActor, public IInteractionPointInterface
{
private:
	GENERATED_BODY()

	CurrentWidgetState m_CurrentWidgetState{CurrentWidgetState::Hidden};
	
	IInteractableInterface* m_pInteractableInterface;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Sets default values for this actor's properties
	AInteractionPoint();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void RegisterParent(IInteractableInterface* pInteractableInterface);

	void EnableInteractor();

	void DisableInteractor();

	void TryRevealWidget();
	
	void TryHideWidget();
	
	void TryFocusWidget();
	
	void TryUnfocusWidget();

	void TryInteract(UInteractionUserComponent* pUser);

	bool HasLinkedInteractable() const;

	bool IsFastInteractable() const;

	FVector GetCurrentLocation() const;

	int GetInteractorId() const;

	bool GetIsEnabled() const;

	void SetIsEnabled(bool enabled);

	bool m_bIsCurrentlyActive{true};

	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	UWidgetComponent* m_pInteractWidget;

	UPROPERTY(EditAnywhere, Category = "Components")
	UBoxComponent* m_TriggerBoxComponent;

	UPROPERTY()
	USceneComponent* m_RootComponent;

	UPROPERTY(EditAnywhere, Category = "Components")
	USceneComponent* m_WidgetAttachmentPoint;
	
	UPROPERTY(EditAnywhere, Category = "Setup")
	int m_interactorId{0};
	
	UPROPERTY(EditAnywhere, Category = "Interaction Settings")
	bool m_bIsQuickInteraction{true};
};
