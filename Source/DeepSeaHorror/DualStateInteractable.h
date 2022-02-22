// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractableInterface.h"
#include "InteractableObjectBase.h"
#include "DualStateInteractable.generated.h"

UENUM()
enum DualStateObjectState
{
	Open,
	Closed
};

DualStateObjectState m_CurrentState = DualStateObjectState::Closed;

UCLASS()
class DEEPSEAHORROR_API ADualStateInteractable : public AInteractableObjectBase
{
	GENERATED_BODY()

public:	
	// Sets default values for this actor's properties
	ADualStateInteractable();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	
	virtual void OnInteractionStarted(UInteractionUserComponent* pInteractionUser) override;

	virtual bool IsInteractionAvailable(const UInteractionUserComponent* pInteractionUser, OUT int type) const override;

	virtual void OnInteractionFinished(UInteractionUserComponent* pInteractionUser) override;

	UPROPERTY(EditAnywhere)
		USkeletalMeshComponent* m_DualStateSkeletalMesh;

	UFUNCTION(BlueprintImplementableEvent)
		void OnChangeState(DualStateObjectState dualStateObjectState);
};
