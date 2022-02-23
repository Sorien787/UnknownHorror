// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractableInterface.h"
#include "InteractableObjectBase.h"
#include "InteractableObjectTypes.h"
#include "InteractableObject.generated.h"

UCLASS()
class DEEPSEAHORROR_API AInteractableObject : public AInteractableObjectBase
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

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintImplementableEvent, Category = "State Switch Event")
		void OnStateChanged();
};
