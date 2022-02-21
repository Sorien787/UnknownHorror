// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractableInterface.h"
#include "InteractableObjectTypes.h"
#include "InteractableObject.generated.h"

UCLASS()
class DEEPSEAHORROR_API AInteractableObject : public AActor, public IInteractableInterface
{
	GENERATED_BODY()
private:

	enum InteractableObjectState
	{
		PickedUp,
		PutDown
	};

	InteractableObjectState m_CurrentState = InteractableObjectState::PutDown;

public:	
	// Sets default values for this actor's properties
	AInteractableObject();

	virtual void OnInteractionStarted(UInteractionUserComponent* pInteractionUser) override;

	virtual void OnInteractionFinished(UInteractionUserComponent* pInteractionUser) override;

	virtual bool IsInteractionAvailable(const UInteractionUserComponent* pInteractionUser, OUT int type) const override;
	
	virtual void OnShowInteractionWidget() override;

	virtual int GetInteractionPriority() override;

	virtual void OnHideInteractionWidget() override;

	virtual FVector GetCurrentLocation() const override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(EditAnywhere)
		int m_InteractionPriority;

	UFUNCTION(BlueprintImplementableEvent, Category = "State Switch Event")
		void OnStateChanged()
};
