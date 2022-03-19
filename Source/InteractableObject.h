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

	bool IsDropActionValid() const;



	bool m_bIsDropActionValid{false};

	FTransform m_RelevantCachedTransform;
	

public:	
	// Sets default values for this actor's properties
	AInteractableObject();
	
	virtual void OnInteractionStarted(UInteractionUserComponent* pInteractionUser, int interactorId) override;

	virtual void OnInteractionFinished(UInteractionUserComponent* pInteractionUser) override;

	virtual bool IsInteractionAvailable(const UInteractionUserComponent* pInteractionUser) const override;

	AInteractionPoint* GetDropInteractionPoint();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintImplementableEvent, Category = "State Switch Event")
		void OnPickUp();

	UFUNCTION(BlueprintImplementableEvent)
		void OnDrop();

	UPROPERTY(EditAnywhere)
		int m_nDropInteractionPointId;
};
